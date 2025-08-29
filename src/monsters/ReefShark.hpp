#pragma once

#include "Monster.hpp"
#include "actions/AttackAction.hpp"
#include "core/Strategy.hpp"

namespace itsamonster {

    struct ReefShark : public Monster {

        struct Melee : public AttackMeleeAction {
            Melee(CombatSystem& system) : AttackMeleeAction(system, "Bite", 4, { std::make_pair(DamageType::Piercing, 7) }, 5, 2) {}
            ~Melee() override = default;

            Advantage HasAdvantage(const Monster& attacker, const Monster& target) const override {
                if (m_system.GetTurnStatusTracker().IsAlliesNear(attacker.GetInstanceId(), target.GetInstanceId(), 5)) {
                    return Advantage::Advantage; // Reef Sharks have advantage if allies are near
                }
                return Advantage::Normal; // Otherwise normal
            }
        };

        ReefShark(CombatSystem& system)
            : Monster(system, "Reef Shark", {
                    std::make_pair(14, 2),
                    std::make_pair(15, 2),
                    std::make_pair(13, 1),
                    std::make_pair(1, -5),
                    std::make_pair(10, 0),
                    std::make_pair(4, -3)
                }), m_attack(system) {
            std::unique_ptr<MeleeApproachAI> ai = std::make_unique<MeleeApproachAI>(system, 5.0);

            SetAI(std::move(ai));
        }

        AttackMeleeAction* GetMeleeAttack() override {
            return &m_attack;
        }

        bool HasDarkvision() const override { return true; }

        int GetHP() const override { return 22; }
        int GetAC() const override { return 12; }
        int GetSpeed() const override { return 30; }

    private:
        Melee m_attack;
    };

} // namespace itsamonster