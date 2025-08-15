#pragma once

#include "Monster.hpp"
#include "Action.hpp"
#include "actions/RechargeAction.hpp"


namespace itsamonster {
struct Rend : public AttackAction {
    Rend() : AttackAction("Rend", 10, 17, DamageType::Slashing, 5) {}
    ~Rend() override = default;
};

struct BreathWeapon : public RechargeAction {
    BreathWeapon() : RechargeAction("Breath Weapon", 5, 30) {}

    void Execute(Monster& attacker, Monster& target, std::mt19937 &rng) override {
        if (!IsAvailable()) {
            LOG(attacker.GetName() << " breath weapon not recharged.");
            return;
        }
        LOG(attacker.GetName() << " uses Breath Weapon! (Recharge 5-6)");
        if (target.SavingThrow(Stat::Dexterity, 17, rng)) {
            target.TakeDamage(DamageType::Fire, 55 / 2, rng);
        } else {
            target.TakeDamage(DamageType::Fire, 55, rng);
        }
        Consume();
    }
};

struct YoungGoldDragon : public Monster {
    BreathWeapon m_breath;

    YoungGoldDragon()
    : Monster("Young Gold Dragon", 178, 18, 80, {
            std::make_pair(23, 6),
            std::make_pair(14, 6),
            std::make_pair(21, 5),
            std::make_pair(16, 3),
            std::make_pair(13, 5),
            std::make_pair(20, 5)
        }) {}

    bool HasDarkvision() const override { return true; }

    bool IsImmune(DamageType damageType) const override {
        if (damageType == DamageType::Fire) return true;
        return Monster::IsImmune(damageType);
    }

    void TakeAction(Monster& target, std::mt19937 &rng) override {
        if (IsCondition(Condition::Incapacitated)) {
            LOG(GetName() << " is incapacitated and cannot take actions!");
            return;
        }
        if (m_breath.IsAvailable()) {
            m_breath.Execute(*this, target, rng);
        }
        else
        {
            // Multiattack (3 rends)
            Rend rendAction;
            for (int i = 0; i < 3; ++i) {
                rendAction.Execute(*this, target, rng);
            }
        }
    }

    void StartTurn(int round, std::mt19937 &rng) override {
        Monster::StartTurn(round,rng);
        m_breath.TryRecharge(rng);
    }
};

} // namespace itsamonster