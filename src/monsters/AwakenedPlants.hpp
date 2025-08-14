#pragma once

#include "Monster.hpp"
#include "Action.hpp"

namespace itsamonster {

struct AwakenedPlants : public Monster {

    struct Rake : public AttackAction {
        Rake() : AttackAction("Rake", 1, 1, DamageType::Slashing, 5) {}
        ~Rake() override = default;
    };

    AwakenedPlants()
        : Monster("Awakened Plants", 10, 9, {
            std::make_pair(3, -4),
            std::make_pair(8, -1),
            std::make_pair(11, 0),
            std::make_pair(10, 0),
            std::make_pair(10, 0),
            std::make_pair(6, -2)
        }) {}

    void TakeAction(Monster &target, std::mt19937 &rng) override {
        // Implement action logic for Yochlol

        Rake rakeAction;
        rakeAction.Execute(*this, target, rng);
    }

    bool IsResistant(DamageType damageType) const override {
        if (damageType == DamageType::Piercing) return true;
        return Monster::IsResistant(damageType);
    }
};

} // namespace itsamonster