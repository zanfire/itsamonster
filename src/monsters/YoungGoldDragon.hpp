#pragma once

#include "Monster.hpp"
#include "actions/AttackAction.hpp"
#include "actions/RechargeAction.hpp"


namespace itsamonster {
struct Rend : public AttackAction {
    Rend() : AttackAction("Rend", 10, 17, DamageType::Slashing, 5) {}
    ~Rend() override = default;
};

struct BreathWeapon : public RechargeAction {
    BreathWeapon() : RechargeAction("Breath Weapon", 5, 30) {}

protected:
    void Execute(Monster& attacker, Monster& target) override {
        if (!IsAvailable()) {
            LOG(attacker.GetName() << " breath weapon not recharged.");
            return;
        }
        LOG(attacker.GetName() << " uses Breath Weapon! (Recharge 5-6)");
        if (target.SavingThrow(Stat::Dexterity, 17)) {
            target.TakeDamage(DamageType::Fire, 55 / 2);
        } else {
            target.TakeDamage(DamageType::Fire, 55);
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

    void TakeAction(Monster& target) override {
        if (m_breath.IsAvailable()) {
            m_breath.Perform(*this, target);
        }
        else
        {
            // Multiattack (3 rends)
            Rend rendAction;
            for (int i = 0; i < 3; ++i) {
                rendAction.Perform(*this, target);
            }
        }
    }

    void StartTurn(int round) override {
        Monster::StartTurn(round);
        m_breath.TryRecharge();
    }
};

} // namespace itsamonster