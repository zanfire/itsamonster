#pragma once

#include "Logger.hpp"
#include "Monster.hpp"

#include <array>
#include <string>
#include <utility>
#include <random>
#include <iostream>

namespace itsamonster {

struct Action {
    Action() = default;
    virtual ~Action() = default;

    virtual void Execute(Monster& attacker, Monster& target, std::mt19937 &rng) = 0;
    virtual bool IsInRange(const Monster& attacker, const Monster& target) const = 0;
};

struct AttackAction : public Action {
    AttackAction(std::string_view name, int attackBonus, int damage, DamageType dt, int range) : m_name(name), m_attackBonus(attackBonus), m_damage(damage), m_damageType(dt), m_range(range) {}
    ~AttackAction() override = default;

    void Execute(Monster& attacker, Monster& target, std::mt19937 &rng) override;

    virtual bool IsInRange(const Monster& attacker, const Monster& target) const override;
    virtual Advantage HasAdvantage(const Monster& attacker, const Monster& target) const;

private:
    std::string_view m_name;
    int m_attackBonus;
    int m_damage;
    DamageType m_damageType;
    int m_range;
};

} // namespace itsamonster