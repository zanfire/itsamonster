#pragma once

#include "Monster.hpp"
#include "actions/AttackAction.hpp"
#include "core/Strategy.hpp"

namespace itsamonster {

    struct FishWarrior : public Monster {

        struct Melee : public AttackMeleeAction {
            Melee(CombatSystem& system) : AttackMeleeAction(system, "Spear", 3, { std::make_pair(DamageType::Piercing, 5) }, 5) {}
            ~Melee() override = default;
        };

        FishWarrior(CombatSystem& system)
            : Monster(system, "Fish Warrior", {
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

        virtual bool HasDarkvision() const { return true; }

    int GetHP() const override { return 18; }
    int GetAC() const override { return 13; }
    int GetSpeed() const override { return 30; }

    private:
        Melee m_attack;
    };

} // namespace itsamonster