#pragma once

#include "Monster.hpp"
#include "actions/AttackAction.hpp"
#include "actions/RechargeAction.hpp"
#include "core/Strategy.hpp"


namespace itsamonster {

    struct WaterElemental : public Monster {

        struct Melee : public AttackMeleeAction {
            Melee(CombatSystem& system) : AttackMeleeAction(system, "Slam", 7, { std::make_pair(DamageType::Bludgeoning, 13) }, 5, 2) {}
            ~Melee() override = default;

            bool OnTurnEvent(TurnEvent ev, EventPayload* p) {
                if (ev == TurnEvent::OnHit) {
                    auto payload = dynamic_cast<AttackRollPayload*>(p);
                    if (payload == nullptr || payload->action != this) return true;
                    if (payload->target->GetMonsterSize() <= MonsterSize::Medium) {
                        LOGGER.LogMonster(*payload->target, "Got hit by slam, prone.");
                        payload->target->SetCondition(Condition::Prone, 1);
                    } else{
                        LOGGER.LogMonster(*payload->target, "Got hit by slam, not prone because too big.");
                    }
                }
                return true;
            }
        };

        struct Whelm : public RechargeAction {
            Whelm(CombatSystem& system, Monster* self) : RechargeAction(system, self, "Whelm", 4, 5) {}
            ~Whelm() override = default;

            bool OnTurnEvent(TurnEvent ev, EventPayload* p) {

                auto result = RechargeAction::OnTurnEvent(ev, p);
                if (ev == TurnEvent::StartTurn) {
                    auto payload = dynamic_cast<MonsterPayload*>(p);
                    if (payload == nullptr || payload->monster != m_self) return true;
                    if (m_grappled != nullptr && m_grappled->IsCondition(Condition::Restrained)) {
                        LOGGER.LogMonster(*m_grappled, "Takes 9 bludgeoning damage from Whelm grapple.");
                        RaiseDamage(*m_self, *m_grappled, { std::make_pair(DamageType::Bludgeoning, 9) });
                    }
                    else {
                        m_grappled = nullptr; // No longer grappled
                    }

                }
                return result;
            }

        protected:
            void Execute(Monster& attacker, Monster& target) override {
                if (!IsAvailable()) {
                    LOG(attacker.GetName() << " whelm not recharged.");
                    return;
                }
                LOG(attacker.GetName() << " uses Whelm! (Recharge 4-6)");
                if (target.SavingThrow(Ability::Dexterity, 15)) {
                    RaiseDamage(attacker, target, { std::make_pair(DamageType::Fire, 22 / 2) });
                }
                else {
                    RaiseDamage(attacker, target, { std::make_pair(DamageType::Fire, 22) });
                    if (target.GetMonsterSize() <= MonsterSize::Large) {
                        LOGGER.LogMonster(target, "Restrained by Whelm.");
                        target.SetCondition(Condition::Restrained, 5);
                        m_grappled = &target;
                    }
                    else {
                        LOGGER.LogMonster(target, "Not restrained by Whelm because too big.");
                    }

                }
                Consume();
            }

            Monster* m_grappled{ nullptr };
        };

        WaterElemental(CombatSystem& system)
            : Monster(system, "Water Elemental", {
                    std::make_pair(18, 4),
                    std::make_pair(14, 2),
                    std::make_pair(18, 4),
                    std::make_pair(5, -3),
                    std::make_pair(10, 0),
                    std::make_pair(8, -1)
                }), m_attack(system), m_whelm(system, this) {
            std::unique_ptr<MeleeApproachAI> ai = std::make_unique<MeleeApproachAI>(system, 5.0);

            SetAI(std::move(ai));
        }

        AttackMeleeAction* GetMeleeAttack() override {
            return &m_attack;
        }

        RechargeAction* GetRechargeAction() override {
            return &m_whelm;
        }

        bool HasDarkvision() const override { return true; }

        // Fixed stats
        int GetHP() const override { return 114; }
        int GetAC() const override { return 14; }
        int GetSpeed() const override { return 90; }
        bool IsResistant(DamageType type) const override {
            return type == DamageType::Acid || type == DamageType::Fire;
        }
        MonsterSize GetMonsterSize() const override { return MonsterSize::Large; }

    private:
        Melee m_attack;
        Whelm m_whelm;
    };

} // namespace itsamonster