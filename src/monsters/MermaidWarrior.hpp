#pragma once

#include "Monster.hpp"
#include "actions/AttackAction.hpp"
#include "core/Strategy.hpp"

namespace itsamonster {

    struct MermaidWarrior : public Monster {

        struct Melee : public AttackMeleeAction {
            Melee(CombatSystem& system) : AttackMeleeAction(system, "Spear", 2, { std::make_pair(DamageType::Piercing, 3), std::make_pair(DamageType::Cold, 2) }, 5) {}
            ~Melee() override = default;
        };

        struct Ranged : public AttackRangedAction {
            Ranged(CombatSystem& system) : AttackRangedAction(system, "Trident", 2, { std::make_pair(DamageType::Piercing, 3), std::make_pair(DamageType::Cold, 2) }, 20, 60) {
            }
            ~Ranged() override {
            }

            bool OnTurnEvent(TurnEvent ev, EventPayload* p) {
                if (ev == TurnEvent::OnHit) {
                    auto payload = dynamic_cast<AttackRollPayload*>(p);
                    if (payload == nullptr || payload->action != this) return true;
                    LOGGER.LogMonster(*payload->target, "Got hit by magic trident, movement reduced by 10.");
                    m_system.GetTurnStatusTracker().GetTurnStatus(payload->target->GetInstanceId())->actions.movement += 10.f;
                }
                return true;
            }
        };

        MermaidWarrior(CombatSystem& system)
            : Monster(system, "Mermaid Warrior", {
                    std::make_pair(13, 1),
                    std::make_pair(10, 0),
                    std::make_pair(11, 0),
                    std::make_pair(11, 0),
                    std::make_pair(10, 0),
                    std::make_pair(8, -1)
                }), m_attack(system), m_ranged(system) {
            std::unique_ptr<RangedKiteAI> ai = std::make_unique<RangedKiteAI>(system, 30, 60);

            SetAI(std::move(ai));
        }

        AttackMeleeAction* GetMeleeAttack() override {
            return &m_attack;
        }

        AttackRangedAction* GetRangedAttack() override {
            return &m_ranged;
        }

        virtual bool HasDarkvision() const { return false; }

        // Fixed stats for this monster
        int GetHP() const override { return 11; }
        int GetAC() const override { return 11; }
        int GetSpeed() const override { return 40; }

    private:
        Melee m_attack;
        Ranged m_ranged;
    };

} // namespace itsamonster