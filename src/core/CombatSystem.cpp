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

bool CombatSystem::NotifyTurnEvent(TurnEvent ev, EventPayload* payload) {
    bool returnValue = true;
    for (auto listener : m_listeners) {
        returnValue &= listener->OnTurnEvent(ev, payload);
        if (!returnValue) {
            LOG("Turn event " << to_string(ev) << " was cancelled by a listener");
            break; // Stop processing if any listener cancels the event
        }
    }
    return returnValue;
}

void CombatSystem::Round() {
    m_round++;
    LOG("Starting round " << m_round);
    NewRoundPayload payload{};
    payload.round = m_round;
    NotifyTurnEvent(TurnEvent::NewRound, &payload);

    std::vector<Monster*> enemies;
    for (auto monster : m_turnStatusTracker.GetTurnOrder()) {
        for (auto m : m_turnStatusTracker.GetTurnOrder()) {
            if (m != monster) enemies.push_back(m);
        }
        if (enemies.empty()) {
            LOG("No enemies found for monster " << monster->GetName() << ", skipping turn.");
            continue; // No enemies to fight, skip this monster's turn
        }
        Turn(*monster, enemies);

        enemies.clear(); // Reset enemies for next monster
    }
}

void CombatSystem::Turn(Monster& monster, std::vector<Monster*> enemies) {
    MonsterPayload payload{};
    payload.monster = &monster;
    NotifyTurnEvent(TurnEvent::StartTurn, &payload);

    auto* ai = monster.GetAI();
    if (ai) {
        ai->TakeTurn(monster, enemies);
    } else {
        LOG_ERROR("Monster " << monster.GetName() << " has no AI assigned.");
    }
    NotifyTurnEvent(TurnEvent::EndTurn, &payload);
}

void CombatSystem::AddMonster(MonsterPtr monster, int initiative, Position pos) {
    MonsterEnterPayload payload{};
    payload.monster = monster;
    payload.spawnPos = pos;
    payload.round = m_round;
    payload.initiative = initiative;
    NotifyTurnEvent(TurnEvent::MonsterEnter, &payload);

    // TODO: instead of using this method use the events system!
    m_battlefield.SetPosition(*monster, pos);
    AddListener(monster);
}
