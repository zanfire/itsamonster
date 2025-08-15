#pragma once

#include "actions/Action.hpp"

namespace itsamonster {
struct AttackAction : public Action {
    AttackAction(std::string_view name, int attackBonus, int damage, DamageType dt, int range) : m_name(name), m_attackBonus(attackBonus), m_damage(damage), m_damageType(dt), m_range(range) {}
    ~AttackAction() override = default;

    virtual bool IsInRange(const Monster& attacker, const Monster& target) const override;
    virtual Advantage HasAdvantage(const Monster& attacker, const Monster& target) const;

protected:
    void Execute(Monster& attacker, Monster& target) override;

    std::string_view m_name;
    int m_attackBonus;
    int m_damage;
    DamageType m_damageType;
    int m_range;
};

struct AttackMeleeAction : public AttackAction {
    AttackMeleeAction(std::string_view name, int attackBonus, int damage, DamageType dt, int range)
        : AttackAction(name, attackBonus, damage, dt, range) {}
    ~AttackMeleeAction() override = default;
};

struct AttackRangedAction : public AttackAction {
    AttackRangedAction(std::string_view name, int attackBonus, int damage, DamageType dt, int range)
        : AttackAction(name, attackBonus, damage, dt, range) {}
    ~AttackRangedAction() override = default;

    virtual Advantage HasAdvantage(const Monster& attacker, const Monster& target) const;
};
} // namespace itsamonster