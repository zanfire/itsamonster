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
    // Size-aware approach: stop when edge-to-edge distance is within close distance
    battlefield.MoveTowardsInSteps(monster, *target, remaining, m_closeDistance, 5.0);
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
        LOGGER.LogMonster(monster, "is close %s, attack. (distance %.2f)", target->GetName().data(), distance);
        m_attackBehaviour.Execute(monster, enemies);
    }
    else if (distance >= 5.0 && distance <= (monster.GetSpeed())) {
        LOGGER.LogMonster(monster, "is far from target %s, moving closer. (distance %.2f)", target->GetName().data(), distance);
        m_moveBehaviour.Execute(monster, enemies);
    } else {
        DashAction dash(m_system);
        LOGGER.LogMonster(monster, "is too far from target %s, moving closer with dash. (distance %.2f)", target->GetName().data(), distance);
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


    auto& battlefield = m_system.GetBattlefield();
    double remaining = static_cast<double>(monster.GetSpeed());
    auto tpOpt = battlefield.GetPosition(target->GetInstanceId());
    auto spOpt = battlefield.GetPosition(monster.GetInstanceId());

    if (!tpOpt || !spOpt) return;

    auto distance = battlefield.GetDistance(monster.GetInstanceId(), target->GetInstanceId());
    if (distance < 5.0) {
        LOGGER.LogMonster(monster, "is close %s, attack. (distance %.2f)", target->GetName().data(), distance);
        m_attackBehaviour.Execute(monster, enemies);
        return;
    }

    Position tp = *tpOpt;
    Position sp = *spOpt;

    auto recompute = [&]() {
        auto curOpt = battlefield.GetPosition(monster.GetInstanceId());
        if (curOpt) sp = *curOpt;
    };

    auto edgeDist = [&]() {
        return battlefield.GetDistance(monster.GetInstanceId(), target->GetInstanceId());
    };

    auto center = Position{ battlefield.GetWidth() / 2, battlefield.GetHeight() / 2 };

    // Helper to move one step combining "away from target" with a bias toward center (away from borders)
    auto stepAwayPreferCenter = [&](double stepFeet, double targetDist) {
        double dCenter = static_cast<double>(Position{sp}.DistanceTo(tp));
        if (dCenter <= 0.0) dCenter = 1.0; // avoid div by zero

        // Unit vector away from target
        double ax = (static_cast<double>(sp.x) - static_cast<double>(tp.x)) / dCenter;
        double ay = (static_cast<double>(sp.y) - static_cast<double>(tp.y)) / dCenter;

        // Unit vector toward center (away from borders)
        double cx = static_cast<double>(center.x - sp.x);
        double cy = static_cast<double>(center.y - sp.y);
        double clen = std::sqrt(cx * cx + cy * cy);
        if (clen > 1e-9) { cx /= clen; cy /= clen; } else { cx = 0.0; cy = 0.0; }

        // Compute how close we are to the nearest border, in feet
        double marginLeft = static_cast<double>(sp.x);
        double marginRight = static_cast<double>(battlefield.GetWidth() - 1 - sp.x);
        double marginTop = static_cast<double>(sp.y);
        double marginBottom = static_cast<double>(battlefield.GetHeight() - 1 - sp.y);
        double minMargin = std::min(std::min(marginLeft, marginRight), std::min(marginTop, marginBottom));

        // Start biasing toward center when within this range of a border
        const double influenceRange = 10.0; // feet
        double w = 0.0;
        if (minMargin < influenceRange) {
            w = (influenceRange - minMargin) / influenceRange; // 0..1
        }

        // Blend directions: always move away from target; add center bias if near border
        double kCenter = 1.5; // strength of center bias
        double dx = ax + kCenter * w * cx;
        double dy = ay + kCenter * w * cy;
        double len = std::sqrt(dx * dx + dy * dy);
        if (len > 1e-9) { dx /= len; dy /= len; }
        else { dx = ax; dy = ay; }

        // Don't overshoot the desired target distance
    // Use edge-to-edge distance for deciding how far to move away
    double dEdge = edgeDist();
    double toMove = std::min(stepFeet, std::min(remaining, std::max(0.0, targetDist - dEdge)));
        if (toMove <= 0.0) return false;

        Position next{
            static_cast<int>(std::round(sp.x + dx * toMove)),
            static_cast<int>(std::round(sp.y + dy * toMove))
        };
        if (next.x == sp.x && next.y == sp.y) {
            int sdx = (dx > 0.1) ? 1 : (dx < -0.1) ? -1 : 0;
            int sdy = (dy > 0.1) ? 1 : (dy < -0.1) ? -1 : 0;
            if (sdx == 0 && sdy == 0) sdx = 1;
            next = Position{ sp.x + sdx, sp.y + sdy };
        }

        battlefield.SetPosition(monster, next);
        remaining -= toMove;
        recompute();
        return true;
    };

    double d = edgeDist();

    // If too close, optionally attack first (opportunity to shoot at close range), then kite away with border awareness
    if (d <= m_minPreferred) {
        m_attackBehaviour.Execute(monster, enemies);
    }

    if (d < m_minPreferred) {
        const double stepFeet = 5.0;
        // Keep stepping away until we reach the minimum preferred distance or run out of movement
        int safety = 64; // prevent infinite loops
        while (remaining > 0.0 && d < m_minPreferred && safety-- > 0) {
            if (!stepAwayPreferCenter(stepFeet, m_minPreferred)) break;
            d = edgeDist();
        }
    } else if (d > m_maxPreferred) {
        // Size-aware approach
    battlefield.MoveTowardsInSteps(monster, *target, remaining, m_maxPreferred, 5.0);
        recompute();
        d = edgeDist();
    }

    // If within max preferred range after movement, attack
    if (d <= m_maxPreferred) {
        m_attackBehaviour.Execute(monster, enemies);
    }
}

