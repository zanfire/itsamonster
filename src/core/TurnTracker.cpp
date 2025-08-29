#include "TurnTracker.hpp"

#include "CombatSystem.hpp"
#include "monsters/Monster.hpp"
#include <numeric>

using namespace itsamonster;

TurnStatusTracker::TurnStatusTracker(CombatSystem& combatSystem)
    : m_system(combatSystem) {
}

TurnStatusTracker::~TurnStatusTracker() {
}

bool TurnStatusTracker::OnTurnEvent(TurnEvent ev, EventPayload* payload) {
    auto monsterPayload = dynamic_cast<MonsterPayload*>(payload);

    if (ev == TurnEvent::MonsterEnter && dynamic_cast<MonsterEnterPayload*>(payload) != nullptr) {
        auto monsterEnterPayload = dynamic_cast<MonsterEnterPayload*>(payload);
        AddMonster(monsterEnterPayload);
    }
    else if (ev == TurnEvent::MonsterDie) {
        // Clean up the turn tracker for the monster
        GetTurnStatus(monsterPayload->monster->GetInstanceId())->dead = true;
    }
    else if (ev == TurnEvent::MonsterLeave) {
        // Clean up the turn tracker for the monster
        m_turnTrackers.erase(monsterPayload->monster->GetInstanceId());
        m_turnOrder.erase(std::remove(m_turnOrder.begin(), m_turnOrder.end(), monsterPayload->monster), m_turnOrder.end());
    } 
    else if (ev == TurnEvent::NewRound) {
        // Reset round state for all monsters
        for (auto& [id, tracker] : m_turnTrackers) {
            tracker.round = m_system.GetCurrentRound();
            tracker.actions = TurnAction{};
        }
    } 
    else if (ev == TurnEvent::StartTurn) {
        return TrackStartTurn(monsterPayload);
    }
    else if (ev == TurnEvent::EndTurn) {
        return TrackEndTurn(monsterPayload);
    }
    else if (ev == TurnEvent::TakeAction)
    {
        auto it = m_turnTrackers.find(monsterPayload->monster->GetInstanceId());
        if (it == m_turnTrackers.end()) {
            // If the monster is not tracked, we can't proceed
            return false;
        }
        if (it->second.actions.action) {
            // If the monster has already taken an action this turn, we can't proceed
            return false;
        }
        it->second.actions.action = true;
    } 
    else if (ev == TurnEvent::TakeBonusAction) {
        auto it = m_turnTrackers.find(monsterPayload->monster->GetInstanceId());
        if (it == m_turnTrackers.end()) {
            // If the monster is not tracked, we can't proceed
            return false;
        }
        if (it->second.actions.bonusAction) {
            // If the monster has already taken a bonus action this turn, we can't proceed
            return false;
        }
        it->second.actions.bonusAction = true;
    } 
    else if (ev == TurnEvent::TakeReaction) {
        auto it = m_turnTrackers.find(monsterPayload->monster->GetInstanceId());
        if (it == m_turnTrackers.end()) {
            // If the monster is not tracked, we can't proceed
            return false;
        }
        if (it->second.actions.reaction) {
            // If the monster has already taken a reaction this turn, we can't proceed
            return false;
        }
        it->second.actions.reaction = true;
    }
    else if (ev == TurnEvent::AfterMove) {
        auto it = m_turnTrackers.find(monsterPayload->monster->GetInstanceId());
        if (it == m_turnTrackers.end()) {
            // If the monster is not tracked, we can't proceed
            return false;
        }
        // Handle movement logic here if needed
        auto movePayload = static_cast<PositionPayload*>(payload);
        if (movePayload && movePayload->oldPos.has_value()) {
            double distanceMoved = movePayload->newPos.DistanceTo(*movePayload->oldPos);
            it->second.actions.movement += distanceMoved;
        }
    }
    else if (ev == TurnEvent::OnApplyCondition) {
        return TrackCondition(dynamic_cast<ConditionEventPayload*>(payload));
    }
    else if (ev == TurnEvent::OnDamageApplied) {
        auto dmgPayload = dynamic_cast<DamagePayload*>(payload);
        if (dmgPayload == nullptr) {
            LOG_ERROR("OnDamageApplied event received without a valid DamagePayload");
            return false;
        }
        if (!TrackDamage(dmgPayload)) {
            LOG_ERROR("Failed to track damage for monster: " << monsterPayload->monster->GetName());
            return false;
        }
    }
    return true;
}

void TurnStatusTracker::AddMonster(MonsterEnterPayload* payload) {
    TurnStatus ctx;
    ctx.self = payload->monster;
    ctx.round = payload->round;
    ctx.initiative = payload->initiative;
    ctx.damageTaken = 0;
    ctx.faction = payload->faction;
    ctx.actions = TurnAction{};
    
    m_turnTrackers[payload->monster->GetInstanceId()] = ctx;

    m_turnOrder.push_back(payload->monster);
    std::sort(m_turnOrder.begin(), m_turnOrder.end(),
        [&](MonsterPtr a, MonsterPtr b) { return m_turnTrackers[a->GetInstanceId()].initiative > m_turnTrackers[b->GetInstanceId()].initiative; });

    LOGGER.LogMonster(*payload->monster, "New monster tracked (initiative %d, spawn position %d %d %d faction %d)", payload->initiative, payload->spawnPos.x, payload->spawnPos.y, payload->spawnPos.z, payload->faction);
}


bool TurnStatusTracker::IsAlliesNear(MonsterInstanceId allieId, MonsterInstanceId enemyId, int range) const {
    auto faction = GetTurnStatus(allieId)->faction;
    auto enemyPos = m_system.GetBattlefield().GetPosition(enemyId);
    for (const auto& [id, status] : m_turnTrackers) {
        if (id == allieId) continue;
        if (status.faction == faction) {
            if (m_system.GetBattlefield().GetPosition(id)->DistanceTo(*enemyPos) <= range) {
                return true;
            }
        }
    }
    return false;
}

bool TurnStatusTracker::TrackCondition(ConditionEventPayload* payload) {
    auto monster = payload->monster;
    if (auto status = GetTurnStatus(monster->GetInstanceId())) {
        if (payload->phase == Phase::After) {
            status->conditions[static_cast<size_t>(payload->condition)] = m_system.GetCurrentRound() + payload->duration;
        }
    }
    return true;
}

bool TurnStatusTracker::TrackStartTurn(MonsterPayload* payload) {
    auto monster = payload->monster;
    if (auto status = GetTurnStatus(monster->GetInstanceId())) {
        status->actions = TurnAction{};
        status->round = m_system.GetCurrentRound();
    }
    return true;
}
bool TurnStatusTracker::TrackEndTurn(MonsterPayload* payload) {
    auto monster = payload->monster;
    if (auto status = GetTurnStatus(monster->GetInstanceId())) {
        int condition = 0;
        for (auto& deadline : status->conditions) {
            if (deadline <= m_system.GetCurrentRound() && deadline != 0) {
                LOGGER.LogMonster(*monster, "condition %s has ended.", to_string(static_cast<Condition>(condition)).data());
                deadline = 0; // Remove expired condition
            }
            ++condition;
        }
    }
    return true;
}

bool TurnStatusTracker::TrackDamage(DamagePayload* payload) {
    auto monster = payload->monster;
    if (auto status = GetTurnStatus(monster->GetInstanceId())) {
        if (payload->phase != DamagePhase::AfterApply) {
            return true;
        }
        std::string damageTypesStr;
        auto amount = std::accumulate(payload->damages.begin(), payload->damages.end(), 0,
            [&](int sum, const std::pair<DamageType, int>& damage) {
                if (!damageTypesStr.empty())
                {
                    damageTypesStr += ", ";
                }
                damageTypesStr = to_string(damage.first);
                damageTypesStr += " (" + std::to_string(damage.second) + ")";
                return sum + damage.second;
            });
        int before = monster->GetHP() - status->damageTaken;
        auto after = before - amount;
        LOGGER.LogMonster(*monster, "takes %d damage (%d -> %d) %s", amount, before, after, damageTypesStr.c_str());
        status->damageTaken += amount;

        if (status->damageTaken >= monster->GetHP()) {
            m_system.NotifyTurnEvent(TurnEvent::MonsterDie, payload);
            // If the monster has taken enough damage to die, we can remove it from the battlefield
            LOGGER.LogMonster(*monster, "has died.");
        }
    }
    return true;
}