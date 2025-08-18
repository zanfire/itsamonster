#include "CombatSystem.hpp"

#include "monsters/Monster.hpp"
#include "reactions/Reaction.hpp"
#include "Strategy.hpp"

#include <algorithm>

using namespace itsamonster;

void CombatSystem::AddListener(TurnEventListener* listener) {
    m_listeners.push_back(listener);
}

void CombatSystem::RemoveListener(TurnEventListener* listener) {
    m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
}

void CombatSystem::NotifyPositionChanged(Monster& monster, std::optional<Position> oldPos, Position newPos) {
    for (auto listener : m_listeners) {
        listener->OnPositionChanged(monster, oldPos, newPos);
    }
}

void CombatSystem::NotifyTurnEvent(Monster& monster, TurnEvent ev, TurnTracker& ctx) {
    for (auto listener : m_listeners) {
        listener->OnTurnEvent(monster, ev, ctx);
    }
}

void CombatSystem::Round() {
    m_round++;
    std::vector<Monster*> initiativeOrder;
    std::transform(m_turnTrackers.begin(), m_turnTrackers.end(), std::back_inserter(initiativeOrder),
                   [](const auto& pair) { return pair.second.self; });
    std::sort(initiativeOrder.begin(), initiativeOrder.end(),
              [this](Monster* a, Monster* b) { return GetTurnTracker(*a)->initiative > GetTurnTracker(*b)->initiative; });

    std::vector<Monster*> enemies;
    for (auto monster : initiativeOrder) {
        auto ctx = GetTurnTracker(*monster);

        if (!ctx) {
            LOG_ERROR("No turn tracker found for monster: " << monster->GetName());
            continue;
        }

        for (auto m : initiativeOrder)
        {
            if (m != monster) enemies.push_back(m);
        }

        Turn(*monster, *ctx, enemies);

        enemies.clear(); // Reset enemies for next monster
    }
}

void CombatSystem::Turn(Monster& monster, TurnTracker& ctx, std::vector<Monster*> enemies) {
    NotifyTurnEvent(monster, TurnEvent::StartTurn, ctx);

    auto* ai = monster.GetAI();
    if (ai) {
        ai->TakeTurn(monster, ctx, enemies);
    } else {
        LOG_ERROR("Monster " << monster.GetName() << " has no AI assigned.");
    }
    NotifyTurnEvent(monster, TurnEvent::EndTurn, ctx);
}

void CombatSystem::TriggerReaction(Monster& self, Monster& other, Reaction& reaction) {
    // check if self has reaction available
    TurnTracker* tracker = GetTurnTracker(self);
    if (tracker && !tracker->resources.reaction) {
        tracker->resources.reaction = true;
        reaction.Trigger(self, other, TriggerType::OnAttack);
        NotifyTurnEvent(self, TurnEvent::AfterAction, *tracker);
    } else {
        LOG(self.GetName() << " has already used their reaction this turn.");
    }
}

TurnTracker* CombatSystem::GetTurnTracker(Monster& monster) {
    auto it = m_turnTrackers.find(monster.GetInstanceId());
    return (it != m_turnTrackers.end()) ? &it->second : nullptr;
}

void CombatSystem::AddMonster(MonsterPtr monster, int initiative, Position pos) {
    m_turnTrackers[monster->GetInstanceId()].self = monster;
    m_turnTrackers[monster->GetInstanceId()].initiative = initiative;
    m_battlefield.SetPosition(*monster, pos);
    AddListener(monster);
}
