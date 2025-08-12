#include "Action.hpp"
#include "Logger.hpp"

using namespace itsamonster;

void AttackAction::Execute(Monster &attacker, Monster &target, std::mt19937 &rng) {
    std::uniform_int_distribution<int> attackRoll(1, 20);
    int attackResult = attackRoll(rng) + m_attackBonus;

    LOG("Executing action: " << m_name << " with attack result: " << attackResult << " " << (attackResult >= target.GetAC() ? "Hit!" : "Miss!"));
    if (attackResult >= target.GetAC()) {
        int damage = m_damage;
        target.TakeReaction(attacker, damage, true, rng);
        target.TakeDamage(m_damageType, damage, rng);
    }
}
