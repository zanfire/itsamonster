#pragma once

#include "Monster.hpp"
#include "actions/AttackAction.hpp"
#include "core/Strategy.hpp"

namespace itsamonster {

    struct FiendBaron : public Monster {

        struct Melee : public AttackMeleeAction {
            Melee(CombatSystem& system) : AttackMeleeAction(system, "Trident", 7, { std::make_pair(DamageType::Bludgeoning, 13) }, 5, 3) {}
            ~Melee() override = default;

            Advantage HasAdvantage(const Monster& attacker, const Monster& target) const override {
                auto adv = AttackAction::HasAdvantage(attacker, target);
                if (m_system.GetTurnStatusTracker().GetTurnStatus(target.GetInstanceId())->damageTaken > 0) {
                    return ResolveAdvantage(adv, Advantage::Advantage); // Sharky Warriors have advantage if target has taken damage
                }
                return ResolveAdvantage(adv, Advantage::Normal); // Otherwise normal
            }
        };

        FiendBaron(CombatSystem& system)
            : Monster(system, "Fiend Baron", {
                    std::make_pair(19, 4),
                    std::make_pair(15, 5),
                    std::make_pair(16, 6),
                    std::make_pair(14, 2),
                    std::make_pair(13, 4),
                    std::make_pair(17, 3)
                }), m_attack(system) {
            std::unique_ptr<MeleeApproachAI> ai = std::make_unique<MeleeApproachAI>(system, 5.0);

            SetAI(std::move(ai));
        }

        AttackMeleeAction* GetMeleeAttack() override {
            return &m_attack;
        }

        bool HasDarkvision() const override { return true; }

        // Fixed stats
        int GetHP() const override { return 76; }
        int GetAC() const override { return 16; }
        int GetSpeed() const override { return 50; }
        bool IsResistant(DamageType type) const override {
            return type == DamageType::Acid || type == DamageType::Cold;
        }

        MonsterSize GetMonsterSize() const override { return MonsterSize::Large; }

    private:
        Melee m_attack;
    };

} // namespace itsamonster