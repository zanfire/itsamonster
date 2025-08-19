#pragma once

#include "actions/Action.hpp"

namespace itsamonster {
struct AttackAction : public Action {
    AttackAction(CombatSystem& system, std::string_view name, int attackBonus, std::vector<std::pair<DamageType, int>> damage, int range)
        : m_system(system), m_name(name), m_attackBonus(attackBonus), m_damages(std::move(damage)), m_range(range) {}
    ~AttackAction() override = default;

    virtual bool IsInRange(const Monster& attacker, const Monster& target) const override;
    virtual Advantage HasAdvantage(const Monster& attacker, const Monster& target) const;
    int GetRange() const { return m_range; }

protected:
    void Execute(Monster& attacker, Monster& target) override;

protected:
    CombatSystem& m_system;
    std::string_view m_name;
    int m_attackBonus;
    std::vector<std::pair<DamageType, int>> m_damages;
    int m_range;
};

struct AttackMeleeAction : public AttackAction {
    AttackMeleeAction(CombatSystem& system, std::string_view name, int attackBonus, std::vector<std::pair<DamageType, int>> damage, int range)
        : AttackAction(system, name, attackBonus, std::move(damage), range) {}
    ~AttackMeleeAction() override = default;
};

struct AttackRangedAction : public AttackAction {
    AttackRangedAction(CombatSystem& system, std::string_view name, int attackBonus, std::vector<std::pair<DamageType, int>> damage, int range)
        : AttackAction(system, name, attackBonus, std::move(damage), range) {}
    ~AttackRangedAction() override = default;

    virtual Advantage HasAdvantage(const Monster& attacker, const Monster& target) const;
};
} // namespace itsamonster