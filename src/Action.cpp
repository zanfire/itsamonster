#include "Action.hpp"
#include "Logger.hpp"
#include "Types.hpp"

using namespace itsamonster;

void AttackAction::Execute(Monster& attacker, Monster& target, std::mt19937 &rng) {
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

    int d20 = D20Test(rng, attackAdvantage);
    int attackResult = d20 + m_attackBonus;
    bool nat20 = (d20 == 20);
    bool nat1 = (d20 == 1);

    bool hit = !nat1 && (nat20 || attackResult >= target.GetAC());
    LOG("Executing action: " << m_name << " roll=" << d20 << (nat20?"(nat20)":"") << (nat1?"(nat1)":"") << " total=" << attackResult << " vs AC " << target.GetAC() << " " << to_string(attackAdvantage) << " " << (hit?"Hit!":"Miss!"));
    if (hit) {
        int damage = m_damage;
        // no crits damage for monster
        //if (nat20) damage *= 2; // simple crit: double base damage (dice not modeled yet)
        target.TakeReaction(attacker, damage, true, rng);
        target.TakeDamage(m_damageType, damage, rng);
    }
}

bool AttackAction::IsInRange(const Monster& attacker, const Monster& target) const {
    double distance = attacker.GetPosition().DistanceTo(target.GetPosition());
    return distance <= m_range;
}

Advantage AttackAction::HasAdvantage(const Monster& attacker, const Monster& target) const {
    return Advantage::Normal;
}
