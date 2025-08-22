#pragma once

#include "Monster.hpp"
#include "actions/AttackAction.hpp"
#include "core/Strategy.hpp"

namespace itsamonster {

    struct SharkyWarrior : public Monster {

        struct Melee : public AttackMeleeAction {
            Melee(CombatSystem& system) : AttackMeleeAction(system, "Claw", 3, { std::make_pair(DamageType::Piercing, 4) }, 5, 2) {}
            ~Melee() override = default;
        };

        SharkyWarrior(CombatSystem& system)
            : Monster(system, "Sharky Warrior", 22, 12, 30, {
                    std::make_pair(13, 1),
                    std::make_pair(10, 0),
                    std::make_pair(11, 0),
                    std::make_pair(11, 0),
                    std::make_pair(10, 0),
                    std::make_pair(8, -1)
                }), m_attack(system) {
            std::unique_ptr<MeleeApproachAI> ai = std::make_unique<MeleeApproachAI>(system, 5.0);

            SetAI(std::move(ai));
        }

        AttackMeleeAction* GetMeleeAttack() override {
            return &m_attack;
        }

        bool HasDarkvision() const override { return true; }

        bool IsResistant(DamageType type) const override {
            // Sharky Warriors are resistant to cold damage
            return type == DamageType::Cold;
        }

    private:
        Melee m_attack;
    };

} // namespace itsamonster