#include "AttackAction.hpp"
#include "monsters/Monster.hpp"
#include "core/CombatSystem.hpp"

#include "Logger.hpp"
#include "Types.hpp"

namespace itsamonster {

    void AttackAction::Execute(Monster& attacker, Monster& target) {
        for (int i = 0; i < m_multiAttackCount; ++i) {
            if (m_multiAttackCount > 1) {
                LOGGER.LogMonster(attacker, "execute multi-attack %d/%d", i + 1, m_multiAttackCount);
            }
            ExecuteSingleAttack(attacker, target);
        }
    }

    void AttackAction::ExecuteSingleAttack(Monster& attacker, Monster& target) {
        if (attacker.IsCondition(Condition::Incapacitated)) {
            LOGGER.LogMonster(attacker, "is incapacitated and cannot take actions!");
            return;
        }
        if (!IsInRange(attacker, target)) {
            LOGGER.LogMonster(attacker, "is out of range for %s (range %d)", m_name.data(), m_range);
            return; // can't attack this action
        }

        AttackRollPayload attackRollPayload{};
        attackRollPayload.monster = &attacker;
        attackRollPayload.action = this;
        attackRollPayload.target = &target;
        attackRollPayload.advantage = HasAdvantage(attacker, target);
        attackRollPayload.attackRoll = 0; // Will be set after the roll
        attackRollPayload.ac = target.GetAC();
        attackRollPayload.phase = Phase::Before;
        if (!m_system.NotifyTurnEvent(TurnEvent::AttackRoll, &attackRollPayload)) {
            return; // Cancelled by listener
        }

        int d20 = GetDice().D20(attackRollPayload.advantage);
        int attackResult = d20 + m_attackBonus;
        bool nat20 = (d20 == 20);
        bool nat1 = (d20 == 1);

        attackRollPayload.attackRoll = attackResult;
        attackRollPayload.phase = Phase::After;
        if (!m_system.NotifyTurnEvent(TurnEvent::AttackRoll, &attackRollPayload)) {
            return; // Cancelled by listener
        }

        bool hit = !nat1 && (nat20 || attackRollPayload.attackRoll >= attackRollPayload.ac);
        LOGGER.LogMonster(attacker, "Executing action: %s roll=%d%s%s total=%d vs AC %d %s %s", m_name.data(), d20, (nat20 ? "(nat20)" : ""), (nat1 ? "(nat1)" : ""), attackResult, target.GetAC(), to_string(attackRollPayload.advantage).data(), (hit ? "Hit!" : "Miss!"));
        if (hit) {
            if (m_system.NotifyTurnEvent(TurnEvent::OnHit, &attackRollPayload)) {
                DamagePayload damagePayload{};
                damagePayload.monster = &target;
                damagePayload.damages = m_damages;
                damagePayload.phase = DamagePhase::BeforeApply;
                if (m_system.NotifyTurnEvent(TurnEvent::OnDamageApplied, &damagePayload)) {
                    damagePayload.phase = DamagePhase::AfterApply;
                    m_system.NotifyTurnEvent(TurnEvent::OnDamageApplied, &damagePayload);
                }
                else {
                    LOGGER.LogMonster(attacker, "Hit cancelled by OnDamageApplied listener.");
                    return; // Cancelled by listener
                }
            }
            else {
                // If any listener returns false, we cancel the hit
                LOGGER.LogMonster(attacker, "Hit cancelled by listener.");
                return;
            }
        }
        else {
            m_system.NotifyTurnEvent(TurnEvent::OnMiss, &attackRollPayload);
        }
    }

    bool AttackAction::IsInRange(const Monster& attacker, const Monster& target) const {
        auto battlefield = m_system.GetBattlefield();
        double distance = battlefield.GetDistance(attacker.GetInstanceId(), target.GetInstanceId());
        return distance <= m_range;
    }

    bool AttackRangedAction::IsInRange(const Monster& attacker, const Monster& target) const {
        auto battlefield = m_system.GetBattlefield();
        double distance = battlefield.GetDistance(attacker.GetInstanceId(), target.GetInstanceId());
        return distance <= m_maxRange;
    }


    Advantage AttackAction::HasAdvantage(const Monster& attacker, const Monster& target) const {
        Advantage adv = Advantage::Normal;
        if (attacker.IsCondition(Condition::Blinded)) {
            LOGGER.LogMonster(attacker, "is blinded, disadvantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Disadvantage);
        }
        if (target.IsCondition(Condition::Blinded)) {
            LOGGER.LogMonster(target, "is blinded, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        }
        if (attacker.IsCondition(Condition::Invisible)) {
            LOGGER.LogMonster(attacker, "is invisible, disadvantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Disadvantage);
        }
        if (target.IsCondition(Condition::Invisible)) {
            LOGGER.LogMonster(target, "is invisible, disadvantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Disadvantage);
        }
        if (target.IsCondition(Condition::Paralyzed)) {
            LOGGER.LogMonster(target, "is paralyzed, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        }
        if (target.IsCondition(Condition::Petrified)) {
            LOGGER.LogMonster(target, "is petrified, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        }
        if (attacker.IsCondition(Condition::Poisoned)) {
            LOGGER.LogMonster(attacker, "is poisoned, disadvantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Disadvantage);
        }
        if (target.IsCondition(Condition::Prone)) {
            auto battlefield = m_system.GetBattlefield();
            double distance = battlefield.GetDistance(attacker.GetInstanceId(), target.GetInstanceId());
            if (distance <= 5.0) {
                LOGGER.LogMonster(target, "is prone and within 5.0 units, advantage applied.");
                adv = ResolveAdvantage(adv, Advantage::Advantage);
            }
            else {
                LOGGER.LogMonster(target, "is prone and beyond 5.0 units, disadvantage applied.");
                adv = ResolveAdvantage(adv, Advantage::Disadvantage);
            }
        }
        if (attacker.IsCondition(Condition::Restrained)) {
            LOGGER.LogMonster(attacker, "is restrained, disadvantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Disadvantage);
        }
        if (target.IsCondition(Condition::Restrained)) {
            LOGGER.LogMonster(target, "is restrained, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        }
        if (target.IsCondition(Condition::Stunned)) {
            LOGGER.LogMonster(target, "is stunned, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        }
        if (target.IsCondition(Condition::Unconscious)) {
            LOGGER.LogMonster(target, "is unconscious, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        }
        return adv;
    }

    Advantage AttackRangedAction::HasAdvantage(const Monster& attacker, const Monster& target) const {
        Advantage adv = AttackAction::HasAdvantage(attacker, target);
        auto battlefield = m_system.GetBattlefield();
        double distance = battlefield.GetDistance(attacker.GetInstanceId(), target.GetInstanceId());
        if (distance <= 5.0 && !attacker.IsCondition(Condition::Incapacitated)) {
            adv = ResolveAdvantage(adv, Advantage::Disadvantage); // close range, disadvantage
        }
        if (distance > m_range && distance <= m_maxRange) {

            LOGGER.LogMonster(attacker, "is out of range for %s (max range %d), disadvantage", m_name.data(), m_maxRange);
            adv = Advantage::Disadvantage; // Out of range, no advantage
        }
        return adv;
    }

} // namespace itsamonster
