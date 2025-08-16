#include "CombatSystem.hpp"

#include "reactions/Reaction.hpp"

#include <algorithm>

using namespace itsamonster;

void CombatSystem::AddListener(TurnEventListener* listener) {
    m_listeners.push_back(listener);
}

void CombatSystem::RemoveListener(TurnEventListener* listener) {
    m_listeners.erase(std::remove(m_listeners.begin(), m_listeners.end(), listener), m_listeners.end());
}

void CombatSystem::NotifyPositionChanged(Monster& monster, const Position& oldPos, const Position& newPos) {
    for (auto listener : m_listeners) {
        listener->OnPositionChanged(monster, oldPos, newPos);
    }
}

void CombatSystem::NotifyTurnEvent(Monster& monster, TurnEvent ev, TurnTracker& ctx) {
    for (auto listener : m_listeners) {
        listener->OnTurnEvent(monster, ev, ctx);
    }
}

void CombatSystem::Round(std::array<Monster*, 2> initiativeOrder) {
    m_round++;
    // Initialize trackers for this round
    for (int i = 0; i < 2; ++i) {
        m_turnTrackers[i].round = m_round;
        m_turnTrackers[i].self = initiativeOrder[i];
        m_turnTrackers[i].resources = {};
    }

    for (int idx = 0; idx < 2; ++idx) {
        Monster* monster = initiativeOrder[idx];
        TurnTracker& ctx = m_turnTrackers[idx];

        NotifyTurnEvent(*monster, TurnEvent::StartTurn, ctx);
        monster->StartTurn(m_round);

        // Movement phase hooks if needed
        // NotifyTurnEvent(*monster, TurnEvent::BeforeMove, ctx);
        // NotifyTurnEvent(*monster, TurnEvent::AfterMove, ctx);

        // Determine opponent (2-creature initiative assumed here)
    Monster* opponent = (monster == initiativeOrder[0]) ? initiativeOrder[1] : initiativeOrder[0];

        // Perform actions (TakeAction requires a target)
        if (opponent) {
            monster->TakeAction(*opponent);
        }
        NotifyTurnEvent(*monster, TurnEvent::AfterAction, ctx);

        monster->EndTurn();
        NotifyTurnEvent(*monster, TurnEvent::EndTurn, ctx);
    }
}

void CombatSystem::TriggerReaction(Monster& self, Monster& other, Reaction& reaction) {
    // check if self has reaction available
    TurnTracker* tracker = GetTurnTracker(self);
    if (tracker && !tracker->resources.usedReaction) {
        tracker->resources.usedReaction = true;
        reaction.Trigger(self, other, TriggerType::OnAttack);
        NotifyTurnEvent(self, TurnEvent::AfterAction, *tracker);
    } else {
        LOG(self.GetName() << " has already used their reaction this turn.");
    }
}

TurnTracker* CombatSystem::GetTurnTracker(Monster& monster) {
    for (auto& tracker : m_turnTrackers) {
        if (tracker.self == &monster) {
            return &tracker;
        }
    }
    return nullptr;
}

const TurnTracker* CombatSystem::GetTurnTracker(const Monster& monster) const {
    for (auto const& tracker : m_turnTrackers) {
        if (tracker.self == &monster) {
            return &tracker;
        }
    }
    return nullptr;
}

bool CombatSystem::HasUsedReaction(const Monster& monster) const {
    auto* t = GetTurnTracker(monster);
    return t ? t->resources.usedReaction : false;
}