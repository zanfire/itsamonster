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
    : Monster(system, "Larvae", 9, 9, 10, {
            std::make_pair(9, -1),
            std::make_pair(9, -1),
            std::make_pair(10, 0),
            std::make_pair(6, -2),
            std::make_pair(10, 0),
            std::make_pair(2, -4)
        }), m_bite(system) {}

    void TakeAction(Monster& target) override {
        m_bite.Perform(*this, target);
    }

    bool IsResistant(DamageType damageType) const override {
        if (damageType == DamageType::Piercing) return true;
        return Monster::IsResistant(damageType);
    }

    bool HasDarkvision() const override { return true; }

private:
    Bite m_bite;
};

} // namespace itsamonster