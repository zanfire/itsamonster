#pragma once

#include "core/CombatSystem.hpp"
#include <vector>

namespace itsamonster {
 
class CombatSystem;
class Monster; // fwd

// Base interface for pluggable monster AI strategies
struct Strategy {
    explicit Strategy(CombatSystem& system) : m_system(system) {}
    virtual ~Strategy() = default;
    // Execute movement and action selection for this turn against target
    virtual void TakeTurn(Monster& monster, TurnTracker& ctx, const std::vector<Monster*>& enemies) = 0;

protected:
    CombatSystem& m_system;
};

struct Behaviour {
    explicit Behaviour(CombatSystem& system) : m_system(system) {}

    // Default behavior is to do nothing
    virtual void Execute(Monster& monster, TurnTracker& ctx, const std::vector<Monster*>& enemies) = 0;

protected:
    CombatSystem& m_system;
};

struct MoveCloseCombatBehaviour : public Behaviour {
    explicit MoveCloseCombatBehaviour(CombatSystem& system, int close) : Behaviour(system), m_closeDistance(close) {}
    // Default move behaviour does nothing
    void Execute(Monster& monster, TurnTracker& ctx, const std::vector<Monster*>& enemies) override;

private:
    int m_closeDistance;
};

struct AttackBehaviour : public Behaviour {
    explicit AttackBehaviour(CombatSystem& system) : Behaviour(system) {}
    // Default move behaviour does nothing
    void Execute(Monster& monster, TurnTracker& ctx, const std::vector<Monster*>& enemies) override;
};

// Simple melee AI: close to melee reach and attack.
class MeleeApproachAI : public Strategy {
public:
    // stopDistance: desired distance to target after moving (defaults to self reach, typically 5ft)
    explicit MeleeApproachAI(CombatSystem& system, double stopDistance = 5.0)
        : Strategy(system), m_attackBehaviour(system), m_moveBehaviour(system, stopDistance) { }
    ~MeleeApproachAI() override = default;

    void TakeTurn(Monster& monster, TurnTracker& ctx, const std::vector<Monster*>& enemies) override;
private:
    AttackBehaviour m_attackBehaviour;
    MoveCloseCombatBehaviour m_moveBehaviour;
};

// Simple ranged AI: keep distance between [minPreferred, maxPreferred].
// If too close, kite away; if too far, approach until within range, then attack.
class RangedKiteAI : public Strategy {
public:
    RangedKiteAI(CombatSystem& system, double minPreferred = 10.0, double maxPreferred = 30.0)
        : Strategy(system), m_minPreferred(minPreferred), m_maxPreferred(std::max(minPreferred, maxPreferred)) {}
    ~RangedKiteAI() override = default;
    void TakeTurn(Monster& monster, TurnTracker& ctx, const std::vector<Monster*>& enemies) override;
private:
    double m_minPreferred{10.0};
    double m_maxPreferred{30.0};
};

} // namespace itsamonster
