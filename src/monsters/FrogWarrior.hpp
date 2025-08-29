#pragma once

#include "Monster.hpp"
#include "actions/AttackAction.hpp"
#include "core/Strategy.hpp"

namespace itsamonster {

struct FrogWarrior : public Monster {

    struct Melee : public AttackMeleeAction {
        Melee(CombatSystem& system) : AttackMeleeAction(system, "Melee", 4, { std::make_pair(DamageType::Slashing, 6), std::make_pair(DamageType::Poison, 2) }, 5) {}
        ~Melee() override = default;
    };

    FrogWarrior(CombatSystem& system)
    : Monster(system, "Frog Warrior", {
            std::make_pair(12, 1),
            std::make_pair(14, 2),
            std::make_pair(13, 1),
            std::make_pair(7, -2),
            std::make_pair(10, 0),
            std::make_pair(7, -2)
        }), m_attack(system) {
        std::unique_ptr<MeleeApproachAI> ai = std::make_unique<MeleeApproachAI>(system, 5.0);

        SetAI(std::move(ai));
    }

    AttackMeleeAction* GetMeleeAttack() override {
        return &m_attack;
    }

    int GetHP() const override { return 11; }
    int GetAC() const override { return 15; }
    int GetSpeed() const override { return 30; }

private:
    Melee m_attack;
};

} // namespace itsamonster