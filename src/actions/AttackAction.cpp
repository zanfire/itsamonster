#include "AttackAction.hpp"
#include "Logger.hpp"
#include "Types.hpp"

using namespace itsamonster;

void AttackAction::Execute(Monster& attacker, Monster& target) {
    if (attacker.IsCondition(Condition::Incapacitated)) {
        LOG(attacker.GetName() << " is incapacitated and cannot take actions!");
        return;
    }
    if (!IsInRange(attacker, target)) {
        LOG(attacker.GetName() << " is out of range for " << m_name << " (range " << m_range << ")");
        return; // can't attack this action
    }

    Advantage attackAdvantage = Advantage::Normal;
    if (target.IsCondition(Condition::Blinded)) {
        attackAdvantage = ResolveAdvantage(attackAdvantage, Advantage::Advantage);
    }
    if (attacker.IsCondition(Condition::Blinded)) {
        attackAdvantage = ResolveAdvantage(attackAdvantage, Advantage::Disadvantage);
    }

    int d20 = GetDice().D20(attackAdvantage);
    int attackResult = d20 + m_attackBonus;
    bool nat20 = (d20 == 20);
    bool nat1 = (d20 == 1);

    bool hit = !nat1 && (nat20 || attackResult >= target.GetAC());
    LOG("Executing action: " << m_name << " roll=" << d20 << (nat20?"(nat20)":"") << (nat1?"(nat1)":"") << " total=" << attackResult << " vs AC " << target.GetAC() << " " << to_string(attackAdvantage) << " " << (hit?"Hit!":"Miss!"));
    if (hit) {
        int damage = m_damage;
        // no crits damage for monster
        //if (nat20) damage *= 2; // simple crit: double base damage (dice not modeled yet)
        target.TakeReaction(attacker, damage, true);
        target.TakeDamage(m_damageType, damage);
    }
}

bool AttackAction::IsInRange(const Monster& attacker, const Monster& target) const {
    double distance = attacker.GetPosition().DistanceTo(target.GetPosition());
    return distance <= m_range;
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
        double distance = attacker.GetPosition().DistanceTo(target.GetPosition());
        if (distance <= 5.0) {
            LOG(target.GetName() << " is prone and within 5.0 units, advantage applied.");
            adv = ResolveAdvantage(adv, Advantage::Advantage);
        } else {
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
    double distance = attacker.GetPosition().DistanceTo(target.GetPosition());
    if (distance <= 5.0 && !attacker.IsCondition(Condition::Incapacitated)) {
        adv = ResolveAdvantage(adv, Advantage::Disadvantage); // close range, disadvantage
    }
    return adv;
}

