#include "AttackAction.hpp"
#include "monsters/Monster.hpp"
#include "core/CombatSystem.hpp"

#include "Logger.hpp"
#include "Types.hpp"

namespace itsamonster {

    void AttackAction::Execute(Monster& attacker, Monster& target) {
        for (int i = 0; i < m_multiAttackCount; ++i) {
            if (m_multiAttackCount > 1) {
                LOG(attacker.GetName() << " execute multi-attack " << (i + 1) << "/" << m_multiAttackCount);
            }
            ExecuteSingleAttack(attacker, target);
        }
    }

    void AttackAction::ExecuteSingleAttack(Monster& attacker, Monster& target) {
        if (attacker.IsCondition(Condition::Incapacitated)) {
            LOG(attacker.GetName() << " is incapacitated and cannot take actions!");
            return;
        }
        if (!IsInRange(attacker, target)) {
            LOG(attacker.GetName() << " is out of range for " << m_name << " (range " << m_range << ")");
            return; // can't attack this action
        }

        AttackRollPayload attackRollPayload{};
        attackRollPayload.monster = &attacker;
        attackRollPayload.target = &target;
        attackRollPayload.advantage = HasAdvantage(attacker, target);
        attackRollPayload.attackRoll = 0; // Will be set after the roll
        attackRollPayload.ac = target.GetAC();
        attackRollPayload.phase = Phase::Before;
        if (!m_system.NotifyTurnEvent(TurnEvent::AttackRoll, &attackRollPayload)) {
            LOG("Attack roll cancelled by listener.");
            return; // Cancelled by listener
        }

        int d20 = GetDice().D20(attackRollPayload.advantage);
        int attackResult = d20 + m_attackBonus;
        bool nat20 = (d20 == 20);
        bool nat1 = (d20 == 1);

        attackRollPayload.attackRoll = attackResult;
        attackRollPayload.phase = Phase::After;
        if (!m_system.NotifyTurnEvent(TurnEvent::AttackRoll, &attackRollPayload)) {
            LOG("Attack roll cancelled by listener.");
            return; // Cancelled by listener
        }

        bool hit = !nat1 && (nat20 || attackRollPayload.attackRoll >= attackRollPayload.ac);
        LOG("Executing action: " << m_name << " roll=" << d20 << (nat20 ? "(nat20)" : "") << (nat1 ? "(nat1)" : "") << " total=" << attackResult << " vs AC " << target.GetAC() << " " << to_string(attackRollPayload.advantage) << " " << (hit ? "Hit!" : "Miss!"));
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
                    LOG("Hit cancelled by OnDamageApplied listener.");
                    return; // Cancelled by listener
                }
            }
            else {
                // If any listener returns false, we cancel the hit
                LOG("Hit cancelled by listener.");
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
            LOG(attacker.GetName() << " is blinded, disadvantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Disadvantage);
        }
        if (target.IsCondition(Condition::Blinded)) {
            LOG(target.GetName() << " is blinded, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        }
        if (attacker.IsCondition(Condition::Invisible)) {
            LOG(attacker.GetName() << " is invisible, disadvantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Disadvantage);
        }
        if (target.IsCondition(Condition::Invisible)) {
            LOG(target.GetName() << " is invisible, disadvantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Disadvantage);
        }
        if (target.IsCondition(Condition::Paralyzed)) {
            LOG(target.GetName() << " is paralyzed, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        }
        if (target.IsCondition(Condition::Petrified)) {
            LOG(target.GetName() << " is petrified, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        }
        if (attacker.IsCondition(Condition::Poisoned)) {
            LOG(attacker.GetName() << " is poisoned, disadvantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Disadvantage);
        }
        if (target.IsCondition(Condition::Prone)) {
            auto battlefield = m_system.GetBattlefield();
            double distance = battlefield.GetDistance(attacker.GetInstanceId(), target.GetInstanceId());
            if (distance <= 5.0) {
                LOG(target.GetName() << " is prone and within 5.0 units, advantage applied.");
                adv = ResolveAdvantage(adv, Advantage::Advantage);
            }
            else {
                LOG(target.GetName() << " is prone and beyond 5.0 units, disadvantage applied.");
                adv = ResolveAdvantage(adv, Advantage::Disadvantage);
            }
        }
        if (attacker.IsCondition(Condition::Restrained)) {
            LOG(attacker.GetName() << " is restrained, disadvantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Disadvantage);
        }
        if (target.IsCondition(Condition::Restrained)) {
            LOG(target.GetName() << " is restrained, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        }
        if (target.IsCondition(Condition::Stunned)) {
            LOG(target.GetName() << " is stunned, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        }
        if (target.IsCondition(Condition::Unconscious)) {
            LOG(target.GetName() << " is unconscious, advantage applied.");
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
            LOG(attacker.GetName() << " is out of normal range for " << m_name << " (max range " << m_maxRange << "), disadvantage");
            adv = Advantage::Disadvantage; // Out of range, no advantage
        }
        return adv;
    }

} // namespace itsamonster
