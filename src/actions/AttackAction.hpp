#pragma once

#include "actions/Action.hpp"
#include "Types.hpp"
#include <vector>

namespace itsamonster {

struct AttackAction : public Action {
    AttackAction(class CombatSystem& system, std::string_view name, int attackBonus, std::vector<std::pair<DamageType, int>> damage, int range, int multiAttackCount)
        : m_system(system), m_name(name), m_attackBonus(attackBonus), m_damages(std::move(damage)), m_range(range), m_multiAttackCount(multiAttackCount) {
        m_system.AddListener(this);
    }
    ~AttackAction() override {
        m_system.RemoveListener(this);
    }

    bool IsInRange(const Monster& attacker, const Monster& target) const override;
    virtual Advantage HasAdvantage(const Monster& attacker, const Monster& target) const;

protected:
    void Execute(Monster& attacker, Monster& target) override;
    void ExecuteSingleAttack(Monster& attacker, Monster& target);

protected:
    class CombatSystem& m_system;
    std::string_view m_name;
    int m_attackBonus;
    std::vector<std::pair<DamageType, int>> m_damages;
    int m_range;
    int m_multiAttackCount{ 1 }; // Default to 1 for single attacks, can be overridden for multi-attacks
};

struct AttackMeleeAction : public AttackAction {
    AttackMeleeAction(CombatSystem& system, std::string_view name, int attackBonus, std::vector<std::pair<DamageType, int>> damage, int range, int multiAttackCount = 1)
        : AttackAction(system, name, attackBonus, std::move(damage), range, multiAttackCount) {}
    ~AttackMeleeAction() override = default;
};

struct AttackRangedAction : public AttackAction {
    AttackRangedAction(CombatSystem& system, std::string_view name, int attackBonus, std::vector<std::pair<DamageType, int>> damage, int range, int maxRange, int multiAttackCount = 1)
        : AttackAction(system, name, attackBonus, std::move(damage), range, multiAttackCount), m_maxRange(maxRange) {}
    ~AttackRangedAction() override = default;

    bool IsInRange(const Monster& attacker, const Monster& target) const override;
    Advantage HasAdvantage(const Monster& attacker, const Monster& target) const override;
private:
    int m_maxRange;
};
} // namespace itsamonster