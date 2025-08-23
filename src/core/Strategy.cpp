#include "Strategy.hpp"

#include "monsters/Monster.hpp"
#include "Battlefield.hpp"
#include "Logger.hpp"
#include "core/CombatSystem.hpp"
#include "actions/AttackAction.hpp"
#include "actions/DashAction.hpp"

using namespace itsamonster;

namespace {
    Monster* SelectTarget(CombatSystem& system, Monster& monster, const std::vector<Monster*>& enemies) {
        MonsterPtr candidate = nullptr;
        // Simple target selection: choose the first valid enemy
        for (auto* enemy : enemies) {
            if (enemy && !system.GetTurnStatusTracker().GetTurnStatus(enemy->GetInstanceId())->dead) {
                if (!candidate) {
                    candidate = enemy;
                }
                else {
                    auto dist1 = system.GetBattlefield().GetDistance(monster.GetInstanceId(), candidate->GetInstanceId());
                    auto dist2 = system.GetBattlefield().GetDistance(monster.GetInstanceId(), enemy->GetInstanceId());
                    if (dist2 < dist1) {
                        candidate = enemy;
                    }
                }
            }
        }
        return candidate;
    }
}

void MoveCloseCombatBehaviour::Execute(Monster& monster, const std::vector<Monster*>& enemies) {
    auto target = SelectTarget(m_system, monster, enemies);
    if (!target) {
        LOG_ERROR("No valid target found for monster: " << monster.GetName());
        return;
    }
    auto& battlefield = m_system.GetBattlefield();
    auto attackerPos = battlefield.GetPosition(monster.GetInstanceId());
    auto targetPosOpt = battlefield.GetPosition(target->GetInstanceId());

    if (!attackerPos) {
        LOG_ERROR("Monster position not found for monster: " << monster.GetName());
        return;
    }
    if (!targetPosOpt) {
        LOG_ERROR("Target position not found for monster: " << target->GetName());
        return;
    }

    auto distance = attackerPos->DistanceTo(*targetPosOpt);

    if (distance <= m_closeDistance) {
        LOG("Monster " << monster.GetName() << " is already within close distance of target "  << target->GetName());
        return; // Already within close distance
    }

    auto& turnStatus = m_system.GetTurnStatusTracker();
    auto ctx = turnStatus.GetTurnStatus(monster.GetInstanceId());
    // Move towards the closest enemy until within close distance
    double remaining = static_cast<double>(monster.GetSpeed()) - ctx->actions.movement;
    battlefield.MoveTowardsInSteps(monster, *targetPosOpt, remaining, m_closeDistance, 5.0);
}

void AttackBehaviour::Execute(Monster& monster, const std::vector<Monster*>& enemies) {
    auto target = SelectTarget(m_system, monster, enemies);
    if (!target) {
        LOG_ERROR("No valid target found for monster: " << monster.GetName());
        return;
    }

    MonsterPayload monsterPayload;
    monsterPayload.monster = &monster;
    auto meleeAttack = monster.GetMeleeAttack();
    if (meleeAttack && meleeAttack->IsInRange(monster, *target)) {
        if (m_system.NotifyTurnEvent(TurnEvent::TakeAction, &monsterPayload)) {
            meleeAttack->Perform(monster, *target);
        }
        m_system.NotifyTurnEvent(TurnEvent::AfterAction, &monsterPayload);
    }
    auto rangedAttack = monster.GetRangedAttack();
    if (rangedAttack && rangedAttack->IsInRange(monster, *target))
    {
        if (m_system.NotifyTurnEvent(TurnEvent::TakeAction, &monsterPayload)) {
            rangedAttack->Perform(monster, *target);
        }
        m_system.NotifyTurnEvent(TurnEvent::AfterAction, &monsterPayload);
    }
}

void MeleeApproachAI::TakeTurn(Monster& monster, const std::vector<Monster*>& enemies) {
    if (enemies.empty()) {
        LOG_ERROR("No enemies to approach for monster: " << monster.GetName());
        return;
    }
    auto target = SelectTarget(m_system, monster, enemies);
    if (!target) {
        LOG_ERROR("No valid target found for monster: " << monster.GetName());
        return;
    }

    auto& battlefield = m_system.GetBattlefield();
    auto distance = battlefield.GetDistance(monster.GetInstanceId(), target->GetInstanceId());
    if (distance < 5.0) {
        m_attackBehaviour.Execute(monster, enemies);
    }
    else if (distance >= 5.0 && distance <= (monster.GetSpeed() - 5)) {
        m_moveBehaviour.Execute(monster, enemies);
    } else {
        DashAction dash(m_system);
        LOG("Monster " << monster.GetName() << " is too far from target " << target->GetName() << ", moving closer with dash.");
        dash.Perform(monster, *target);
    }
    m_attackBehaviour.Execute(monster, enemies);
}


void RangedKiteAI::TakeTurn(Monster& monster, const std::vector<Monster*>& enemies) {
    auto target = SelectTarget(m_system, monster, enemies);
    if (!target) {
        LOG_ERROR("No valid target found for monster: " << monster.GetName());
        return;
    }
    double remaining = static_cast<double>(monster.GetSpeed());
    auto tpOpt = m_system.GetBattlefield().GetPosition(target->GetInstanceId());
    auto spOpt = m_system.GetBattlefield().GetPosition(monster.GetInstanceId());

    if (!tpOpt || !spOpt) return;

    Position tp = *tpOpt;
    Position sp = *spOpt;
    double d = sp.DistanceTo(tp);

    // move to desirred range, and after move to far away
    if (d <= m_minPreferred) {
        m_attackBehaviour.Execute(monster, enemies);
    }

    if (d < m_minPreferred) {
        m_system.GetBattlefield().MoveAwayInSteps(monster, tp, remaining, m_minPreferred, 5.0);
    } else if (d > m_maxPreferred) {
        m_system.GetBattlefield().MoveTowardsInSteps(monster, tp, remaining, m_maxPreferred, 5.0);
    }

    if (d <= m_maxPreferred) {
        m_attackBehaviour.Execute(monster, enemies);
    }
}

