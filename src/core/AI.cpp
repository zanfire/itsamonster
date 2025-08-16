#include "AI.hpp"

#include "monsters/Monster.hpp"
#include "Battlefield.hpp"
#include "Logger.hpp"
#include "core/CombatSystem.hpp"

namespace itsamonster {

void MeleeApproachAI::TakeTurn(Monster& self, Monster& target) {
    // Move towards until within stop distance (default: reach)
    double remaining = static_cast<double>(self.GetSpeed());
    double desired = m_stopDistance > 0 ? m_stopDistance : static_cast<double>(self.GetReach());
    CombatSystem::Instance().GetBattlefield().MoveTowardsInSteps(self, target.GetPosition(), remaining, desired, 5.0);

    // Then perform monster's own action selection
    self.TakeAction(target);
}

void RangedKiteAI::TakeTurn(Monster& self, Monster& target) {
    double remaining = static_cast<double>(self.GetSpeed());
    Position tp = target.GetPosition();
    Position sp = self.GetPosition();
    double d = sp.DistanceTo(tp);

    if (d < m_minPreferred) {
        CombatSystem::Instance().GetBattlefield().MoveAwayInSteps(self, tp, remaining, m_minPreferred, 5.0);
    } else if (d > m_maxPreferred) {
        CombatSystem::Instance().GetBattlefield().MoveTowardsInSteps(self, tp, remaining, m_maxPreferred, 5.0);
    }

    self.TakeAction(target);
}

} // namespace itsamonster
