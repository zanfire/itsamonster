#pragma once

#include "Monster.hpp"
#include "actions/AttackAction.hpp"
#include "core/Strategy.hpp"

namespace itsamonster {

struct AwakenedPlants : public Monster {

    struct Rake : public AttackMeleeAction {
        Rake(CombatSystem& system) : AttackMeleeAction(system, "Rake", 1, { std::make_pair(DamageType::Slashing, 1) }, 5) {}
        ~Rake() override = default;
    };

    AwakenedPlants(CombatSystem& system)
    : Monster(system, "Awakened Plants", {
            std::make_pair(3, -4),
            std::make_pair(8, -1),
            std::make_pair(11, 0),
            std::make_pair(10, 0),
            std::make_pair(10, 0),
            std::make_pair(6, -2)
        }), m_rake(system) {
        std::unique_ptr<MeleeApproachAI> ai = std::make_unique<MeleeApproachAI>(system, 5.0);

        SetAI(std::move(ai));
    }

    AttackMeleeAction* GetMeleeAttack() override {
        return &m_rake;
    }

    bool IsResistant(DamageType damageType) const override {
        if (damageType == DamageType::Piercing) return true;
        return Monster::IsResistant(damageType);
    }

    int GetHP() const override { return 10; }
    int GetAC() const override { return 9; }
    int GetSpeed() const override { return 20; }

private:
    Rake m_rake;
};

} // namespace itsamonster