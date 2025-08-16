#pragma once

#include <memory>
#include <utility>
#include <cmath>

namespace itsamonster {

class Monster; // fwd

// Base interface for pluggable monster AI strategies
struct IAIStrategy {
    virtual ~IAIStrategy() = default;
    // Execute movement and action selection for this turn against target
    virtual void TakeTurn(Monster& self, Monster& target) = 0;
};

// Simple melee AI: close to melee reach and attack.
class MeleeApproachAI : public IAIStrategy {
public:
    // stopDistance: desired distance to target after moving (defaults to self reach, typically 5ft)
    explicit MeleeApproachAI(double stopDistance = 5.0) : m_stopDistance(stopDistance) {}
    void TakeTurn(Monster& self, Monster& target) override;
private:
    double m_stopDistance{5.0};
};

// Simple ranged AI: keep distance between [minPreferred, maxPreferred].
// If too close, kite away; if too far, approach until within range, then attack.
class RangedKiteAI : public IAIStrategy {
public:
    RangedKiteAI(double minPreferred = 10.0, double maxPreferred = 30.0)
        : m_minPreferred(minPreferred), m_maxPreferred(std::max(minPreferred, maxPreferred)) {}
    void TakeTurn(Monster& self, Monster& target) override;
private:
    double m_minPreferred{10.0};
    double m_maxPreferred{30.0};
};

} // namespace itsamonster
