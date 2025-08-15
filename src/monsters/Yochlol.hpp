#pragma once

#include "Monster.hpp"
#include "Action.hpp"

namespace itsamonster {

struct Yochlol : public Monster {
    struct CausticLash : public AttackAction {
        CausticLash() : AttackAction("Caustic Lash", 8, 25, DamageType::Acid, 5) {}
        ~CausticLash() override = default;
    };

    Yochlol()
    : Monster("Yochlol", 153, 15, 30, {
            std::make_pair(15, 6),
            std::make_pair(19, 6),
            std::make_pair(18, 5),
            std::make_pair(13, 3),
            std::make_pair(15, 5),
            std::make_pair(17, 5)
        }) {}

    bool HasDarkvision() const override { return true; }

    void TakeAction(Monster& target, std::mt19937 &rng) override {
        CausticLash causticLashAction;
        for (int i = 0; i <= 1; ++i) {
            causticLashAction.Execute(*this, target, rng);
        }
    }

    bool IsResistant(DamageType damageType) const override {
        if (damageType == DamageType::Fire) return true;
        if (damageType == DamageType::Cold) return true;
        if (damageType == DamageType::Lightning) return true;
        return Monster::IsResistant(damageType);
    }

    void TakeReaction(Monster& attacker, int damage, bool ishit, std::mt19937 &rng) override {
        if (m_round.reaction) {
            LOG(GetName() << " has already taken a reaction this turn!");
            return;
        }
        if (ishit) {
            m_round.reaction = true;
            LOG(GetName() << " takes a reaction to " << attacker.GetName() << "'s attack!");
            if (!attacker.SavingThrow(Stat::Constitution, 15, rng)) {
                attacker.SetCondition(Condition::Incapacitated, m_round.rounds + 2);
            }
        }
    }
};

} // namespace itsamonster