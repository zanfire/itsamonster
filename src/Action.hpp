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

    virtual void Execute(Monster &attacker, Monster &target, std::mt19937 &rng) = 0;
};

struct AttackAction : public Action {
    AttackAction(std::string n, int a, int d, DamageType dt, int r) : m_name(std::move(n)), m_attackBonus(a), m_damage(d), m_damageType(dt), m_range(r) {}
    ~AttackAction() override = default;

    void Execute(Monster &attacker, Monster &target, std::mt19937 &rng) override;

private:
    std::string m_name;
    int m_attackBonus;
    int m_damage;
    DamageType m_damageType;
    int m_range;
};

} // namespace itsamonster