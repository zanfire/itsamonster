#include "Strategy.hpp"

#include "monsters/Monster.hpp"
#include "Battlefield.hpp"
#include "Logger.hpp"
#include "core/CombatSystem.hpp"
#include "actions/AttackAction.hpp"

namespace itsamonster {

void MoveCloseCombatBehaviour::Execute(Monster& monster, TurnTracker& ctx, const std::vector<Monster*>& enemies) {
    auto& battlefield = m_system.GetBattlefield();
    auto attackerPos = battlefield.GetPosition(monster.GetInstanceId());
    auto targetPosOpt = battlefield.GetPosition(enemies[0]->GetInstanceId());

    if (!attackerPos) {
        LOG_ERROR("Monster position not found for monster: " << monster.GetName());
        return;
    }
    if (!targetPosOpt) {
        LOG_ERROR("Target position not found for monster: " << enemies[0]->GetName());
        return;
    }

    auto distance = attackerPos->DistanceTo(*targetPosOpt);

    if (distance <= m_closeDistance) {
        LOG("Monster " << monster.GetName() << " is already within close distance of target "  << enemies[0]->GetName());
        return; // Already within close distance
    }
    // Move towards the closest enemy until within close distance
    double remaining = static_cast<double>(monster.GetSpeed()) - ctx.resources.movement;

    ctx.resources.movement += battlefield.MoveTowardsInSteps(monster, *targetPosOpt, remaining, m_closeDistance, 5.0);
}

void AttackBehaviour::Execute(Monster& monster, TurnTracker& ctx, const std::vector<Monster*>& enemies) {
    auto& battlefield = m_system.GetBattlefield();
    auto distance = battlefield.GetDistance(monster.GetInstanceId(), enemies[0]->GetInstanceId());

    auto meleeAttack = monster.GetMeleeAttack();
    if (meleeAttack && distance <= meleeAttack->GetRange()) {
        m_system.NotifyTurnEvent(monster, TurnEvent::BeforeAction, ctx);
        meleeAttack->Perform(monster, *enemies[0]);
        m_system.NotifyTurnEvent(monster, TurnEvent::AfterAction, ctx);
    }
    auto rangedAttack = monster.GetRangedAttack();
    if (rangedAttack && distance <= rangedAttack->GetRange())
    {
        m_system.NotifyTurnEvent(monster, TurnEvent::BeforeAction, ctx);
        meleeAttack->Perform(monster, *enemies[0]);
        m_system.NotifyTurnEvent(monster, TurnEvent::AfterAction, ctx);
    }
}

void MeleeApproachAI::TakeTurn(Monster& monster, TurnTracker& ctx, const std::vector<Monster*>& enemies) {
    if (enemies.empty()) {
        LOG_ERROR("No enemies to approach for monster: " << monster.GetName());
        return;
    }
    
    m_attackBehaviour.Execute(monster, ctx, enemies);
    m_moveBehaviour.Execute(monster, ctx, enemies);
    m_attackBehaviour.Execute(monster, ctx, enemies);
}


void RangedKiteAI::TakeTurn(Monster& monster, TurnTracker& ctx, const std::vector<Monster*>& enemies) {
    double remaining = static_cast<double>(monster.GetSpeed());
    auto tpOpt = m_system.GetBattlefield().GetPosition(enemies[0]->GetInstanceId());
    auto spOpt = m_system.GetBattlefield().GetPosition(monster.GetInstanceId());

    if (!tpOpt || !spOpt) return;

    Position tp = *tpOpt;
    Position sp = *spOpt;
    double d = sp.DistanceTo(tp);

    if (d < m_minPreferred) {
        m_system.GetBattlefield().MoveAwayInSteps(monster, tp, remaining, m_minPreferred, 5.0);
    } else if (d > m_maxPreferred) {
        m_system.GetBattlefield().MoveTowardsInSteps(monster, tp, remaining, m_maxPreferred, 5.0);
    }

    monster.TakeAction(*enemies[0]);
}

} // namespace itsamonster
