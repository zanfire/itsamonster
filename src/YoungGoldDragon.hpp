#pragma once

#include "Monster.hpp"
#include "Action.hpp"

namespace itsamonster {
struct Rend : public AttackAction {
    Rend() : AttackAction("Rend", 10, 17, DamageType::Slashing, 5) {}
    ~Rend() override = default;
};

struct BreathWeapon : public Action {
    BreathWeapon() : Action() {}

    virtual void Execute(Monster &attacker, Monster &target, std::mt19937 &rng) override {
        if (target.SavingThrow(Stat::Dexterity, 17, rng)) {
            target.TakeDamage(DamageType::Fire, 55 / 2, rng);
        } else {
            target.TakeDamage(DamageType::Fire, 55, rng);
        }
    }
};

struct YoungGoldDragon : public Monster {
    bool m_hasUsedBreathWeapon = true;

    YoungGoldDragon()
        : Monster("Young Gold Dragon", 178, 18, {
            std::make_pair(Stat::Strength, std::make_pair(23, 6)),
            std::make_pair(Stat::Dexterity, std::make_pair(14, 6)),
            std::make_pair(Stat::Constitution, std::make_pair(21, 5)),
            std::make_pair(Stat::Intelligence, std::make_pair(16, 3)),
            std::make_pair(Stat::Wisdom, std::make_pair(13, 5)),
            std::make_pair(Stat::Charisma, std::make_pair(20, 5))
        }) {}

    void TakeAction(Monster &target, std::mt19937 &rng) override {
        if (IsCondition(Condition::Incapacitated)) {
            LOG(GetName() << " is incapacitated and cannot take actions!");
            return;
        }
        if (m_hasUsedBreathWeapon) {
            BreathWeapon breathWeaponAction;
            breathWeaponAction.Execute(*this, target, rng);
            m_hasUsedBreathWeapon = false;
            return; // Skip other actions if breath weapon is used
        } else {
            Rend rendAction;
            for (int i = 0; i <= 2; ++i) {
                if (IsCondition(Condition::Incapacitated)) {
                    LOG(GetName() << " is incapacitated and cannot take actions!");
                    return;
                }
                rendAction.Execute(*this, target, rng);
            }
        }
    }

    void StartTurn(int round, std::mt19937 &rng) override {
        Monster::StartTurn(round,rng);
        if (!m_hasUsedBreathWeapon) {
            std::uniform_int_distribution<int> roll(1, 6);
            if (roll(rng) >= 5) {
                m_hasUsedBreathWeapon = true;
                LOG(GetName() << " regains use of Breath Weapon!");
            }
        }
    }
};

} // namespace itsamonster