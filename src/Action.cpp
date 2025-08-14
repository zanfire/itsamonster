#include "Action.hpp"
#include "Logger.hpp"
#include "Types.hpp"

using namespace itsamonster;

void AttackAction::Execute(Monster &attacker, Monster &target, std::mt19937 &rng) {
    Advantage attackAdvantage = Advantage::Normal;
    if (target.IsCondition(Condition::Blinded)) {
        attackAdvantage = ResolveAdvantage(attackAdvantage, Advantage::Advantage);
    }
    if (attacker.IsCondition(Condition::Blinded)) {
        attackAdvantage = ResolveAdvantage(attackAdvantage, Advantage::Disadvantage);
    }
    int attackResult = D20Test(rng, attackAdvantage) + m_attackBonus;

    LOG("Executing action: " << m_name << " with attack result: " << attackResult << " " << to_string(attackAdvantage) << " " << (attackResult >= target.GetAC() ? "Hit!" : "Miss!"));
    if (attackResult >= target.GetAC()) {
        int damage = m_damage;
        target.TakeReaction(attacker, damage, true, rng);
        target.TakeDamage(m_damageType, damage, rng);
    }
}
