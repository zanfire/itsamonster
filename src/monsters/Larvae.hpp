#pragma once

#include "Monster.hpp"
#include "actions/AttackAction.hpp"

namespace itsamonster {

struct Larvae : public Monster {
    struct Bite : public AttackMeleeAction {
        Bite(CombatSystem& system) : AttackMeleeAction(system, "Bite", 1, { std::make_pair(DamageType::Necrotic, 1) }, 5) {
        }
        ~Bite() override = default;
    };
    Larvae(CombatSystem& system)
    : Monster(system, "Larvae", {
            std::make_pair(9, -1),
            std::make_pair(9, -1),
            std::make_pair(10, 0),
            std::make_pair(6, -2),
            std::make_pair(10, 0),
            std::make_pair(2, -4)
        }), m_bite(system) {}

    bool IsResistant(DamageType damageType) const override {
        if (damageType == DamageType::Piercing) return true;
        return Monster::IsResistant(damageType);
    }

    bool HasDarkvision() const override { return true; }

    int GetHP() const override { return 9; }
    int GetAC() const override { return 9; }
    int GetSpeed() const override { return 10; }

private:
    Bite m_bite;
};

} // namespace itsamonster