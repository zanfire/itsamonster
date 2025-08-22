#pragma once

#include "actions/Action.hpp"
#include "core/CombatSystem.hpp"

namespace itsamonster {

// Dash action: spend your action to move up to your speed (again) this turn.
// This implementation performs the movement immediately towards the target,
// up to the attacker's speed, stopping at an optional stop distance.
struct DashAction : public Action {
    explicit DashAction(CombatSystem& system, std::string_view name = "Dash", double stopDistance = 0.0)
        : m_system(system), m_name(name), m_stopDistance(stopDistance) {}
    ~DashAction() override = default;

    // Dash does not require range to execute; always allowed.
    bool IsInRange(const Monster& /*attacker*/, const Monster& /*target*/) const override { return true; }

protected:
    void Execute(Monster& attacker, Monster& target) override;

private:
    CombatSystem& m_system;
    std::string_view m_name;
    double m_stopDistance{0.0};
};

} // namespace itsamonster
