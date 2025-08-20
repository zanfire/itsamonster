#include "Monster.hpp"

#include "core/CombatSystem.hpp"
#include "core/TurnTracker.hpp"
#include <iomanip>

using namespace itsamonster;

// Initialize the global atomic instance ID counter (starts at 0 so first assigned is 1)
std::atomic<MonsterInstanceId> Monster::s_nextId{ 0 };

Monster::~Monster() = default;

bool Monster::IsCondition(Condition condition) const {
    if (auto status = m_system.GetTurnStatusTracker().GetTurnStatus(GetInstanceId())) {
        // Check if the condition is active
        return status->conditions[int(condition)] > 0;
    }
    return false;
}
void Monster::SetCondition(Condition condition, int duration) {
    if (duration <= 0) {
        LOG(m_name << " tried to set condition " << to_string(condition) << " with non-positive duration, ignoring.");
        return; // Invalid duration
    }
    ConditionEventPayload payload{};
    payload.monster = this;
    payload.condition = condition;
    payload.phase = Phase::Before;
    payload.duration = duration;
    if (m_system.NotifyTurnEvent(TurnEvent::OnApplyCondition, &payload)) {
        payload.phase = Phase::After;
        // Only apply condition if the Before phase was accepted
        m_system.NotifyTurnEvent(TurnEvent::OnApplyCondition, &payload);
        LOG(m_name << " applied condition " << to_string(condition) << " for " << duration << " rounds.");
    } else {
        LOG(m_name << " failed to apply condition " << to_string(condition) << ", cancelled by listener.");
        return; // Cancelled by listener
    }
}

bool Monster::SavingThrow(Ability stat, int DC) {
    int result = GetDice().Roll(20) + m_stats[int(stat)].second;
    if (result >= DC) {
        LOG(m_name << " succeeds the saving throw against " << to_string(stat));
        return true;
    } else {
        LOG(m_name << " fails the saving throw against " << to_string(stat));
    }
    return false;
}

void Monster::TakeDamage(DamageType type, int damage) {}

bool Monster::OnPositionChanged(Monster& monster, std::optional<Position> oldPos, Position newPos) {
    if (&monster != this) return true;
    std::cout << std::fixed << std::setprecision(2);
    LOG(monster.GetName() << " moved from " << (oldPos ? std::to_string(oldPos->x) + "," + std::to_string(oldPos->y) : "unknown")
        << " to " << newPos.x << "," << newPos.y);
    return true;
}

bool Monster::OnTurnEvent(TurnEvent ev, EventPayload* payload) {
    auto monsterPayload = dynamic_cast<MonsterPayload*>(payload);
    if (monsterPayload == nullptr || monsterPayload->monster != this) return true;

    switch (ev) {
    case TurnEvent::StartTurn:
        LOG(monsterPayload->monster->GetName() << " starts their turn.");
        break;
    case TurnEvent::EndTurn:
        LOG(monsterPayload->monster->GetName() << " ends their turn.");
        break;
    case TurnEvent::BeforeAction:
        LOG(monsterPayload->monster->GetName() << " is about to act.");
        break;
    case TurnEvent::AfterAction:
        LOG(monsterPayload->monster->GetName() << " has completed their action.");
        break;
    case TurnEvent::OnDamageApplied:
    {
        auto dmgPayload = dynamic_cast<DamagePayload*>(payload);
        if (dmgPayload) {
            return OnDamageApplied(dmgPayload);
        }
        break;
    }
    case TurnEvent::OnApplyCondition:
        return OnApplyCondition(dynamic_cast<ConditionEventPayload*>(payload));
        break;
    }
    return true;
}

bool Monster::OnApplyCondition(ConditionEventPayload* payload) {
    if (payload == nullptr) {
        LOG(m_name << " received null condition payload, ignoring.");
        return true; // Nothing to do
    }
    if (payload->monster != this) return true; // Not our condition
    if (payload->phase == Phase::Before) {
        LOG(m_name << " is about to apply condition " << to_string(payload->condition)
            << " for " << payload->duration << " rounds.");
        if (IsImmune(payload->condition)) {
            LOG(m_name << " is immune to " << to_string(payload->condition) << ", condition not applied.");
            return false; // Skip applying condition
        }
    }
    return true;
}

bool Monster::OnDamageApplied(DamagePayload* payload) {
    if (payload->monster != this) return true; // Not our damage
    if (payload->phase != DamagePhase::AfterApply) {
        LOG(m_name << " received damage before application phase, skipping immunity/resistance checks.");
        return true; // Only handle after-apply phase
    }
    for (auto& [type, amount] : payload->damages) {
        if (IsImmune(type)) {
            amount = 0;
            LOG(m_name << " is immune to " << to_string(type) << ", no damage taken.");
        }
        if (IsResistant(type)) {
            amount /= 2;
            LOG(m_name << " is resistant to " << to_string(type) << ", damage halved to " << amount );
        }
        if (IsVulnerable(type)) {
            amount *= 2;
            LOG(m_name << " is vulnerable to " << to_string(type) << ", damage doubled to " << amount);
        }
    }
    return true;
}