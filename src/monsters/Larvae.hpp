#pragma once

#include "Monster.hpp"
#include "actions/AttackAction.hpp"

namespace itsamonster {

struct Larvae : public Monster {
    struct Bite : public AttackAction {
        Bite() : AttackAction("Bite", 1, 1, DamageType::Necrotic, 5) {}
        ~Bite() override = default;
    };
    Larvae()
    : Monster("Larvae", 9, 9, 10, {
            std::make_pair(9, -1),
            std::make_pair(9, -1),
            std::make_pair(10, 0),
            std::make_pair(6, -2),
            std::make_pair(10, 0),
            std::make_pair(2, -4)
        }) {}

    void TakeAction(Monster& target) override {
        Bite biteAction;
        biteAction.Perform(*this, target);
    }

    bool IsResistant(DamageType damageType) const override {
        if (damageType == DamageType::Piercing) return true;
        return Monster::IsResistant(damageType);
    }

    bool HasDarkvision() const override { return true; }
};

} // namespace itsamonster