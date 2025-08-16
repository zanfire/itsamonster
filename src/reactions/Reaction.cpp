#include "Reaction.hpp"
#include "Logger.hpp"
#include "Types.hpp"

using namespace itsamonster;

OpportunityAttackReaction::OpportunityAttackReaction(Monster& monster) : Reaction(monster, "Opportunity Attack") {
    CombatSystem::Instance().AddListener(this);
}

OpportunityAttackReaction::~OpportunityAttackReaction() {
    CombatSystem::Instance().RemoveListener(this);
}

bool OpportunityAttackReaction::IsTriggered(TriggerType triggerType) const {
    return triggerType == TriggerType::OnEnterReach || triggerType == TriggerType::OnExitReach;
}

void OpportunityAttackReaction::Trigger(Monster& self, Monster& other, TriggerType triggerType) {
    //self.GetMeleeAttack().Attack(other);
}

void OpportunityAttackReaction::OnPositionChanged(Monster& monster, const Position& oldPos, const Position& newPos) {
    // Check if the monster moved out of reach
    if (m_monster.GetPosition().DistanceTo(oldPos) <= m_monster.GetReach()
        && m_monster.GetPosition().DistanceTo(newPos) > m_monster.GetReach()) {
        LOG(monster.GetName() << " moved out reach, triggering opportunity attack.");
        CombatSystem::Instance().TriggerReaction(m_monster, monster, *this);
    }
}
