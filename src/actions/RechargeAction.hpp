#pragma once

#include "Logger.hpp"
#include "Action.hpp"
#include "core/CombatSystem.hpp"

#include <array>
#include <string>
#include <utility>
#include <random>
#include <iostream>

namespace itsamonster {

// RechargeAction models actions that require a recharge roll at start of turn (e.g., Recharge 5-6)
struct RechargeAction : public Action {
    RechargeAction(CombatSystem& system, Monster* self, std::string_view n, int rechargeMin, int range) : m_system(system), m_self(self), m_name(n), m_rechargeMin(rechargeMin), m_range(range) {
        m_system.AddListener(this);
    }
    ~RechargeAction() override {
        m_system.RemoveListener(this);
    }

    bool IsAvailable() const { return m_available; }
    void TryRecharge();
    void Consume() { m_available = false; }
    bool IsInRange(const Monster& attacker, const Monster& target) const override;
    void RaiseDamage(Monster& attacker, Monster& target, std::vector<std::pair<DamageType, int>> damage);

    bool OnTurnEvent(TurnEvent ev, EventPayload* p) override;
protected:
    CombatSystem& m_system;
    Monster* m_self{ nullptr };
    std::string_view m_name;
    int m_rechargeMin{ 0 }; // e.g., 5 for 'Recharge 5-6'
    bool m_available{ true };
    int m_range{ 0 };
};

} // namespace itsamonster