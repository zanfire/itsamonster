#pragma once

#include <cmath>

namespace itsamonster {

struct Position {
    int x{0};
    int y{0};
    int z{0}; // Optional, for 3D support

    double DistanceTo(const Position &other) const {
        int dx = x - other.x;
        int dy = y - other.y;
        int dz = z - other.z;
        return std::sqrt(static_cast<double>(dx*dx + dy*dy + dz*dz));
    }
};

class Monster; // forward declaration

class Battlefield {
public:
    Battlefield(int w, int h) : m_width(w), m_height(h) {}

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    Position Clamp(Position p) const {
        if (p.x < 0) p.x = 0; if (p.x >= m_width) p.x = m_width - 1;
        if (p.y < 0) p.y = 0; if (p.y >= m_height) p.y = m_height - 1;
        return p;
    }

        // Movement helpers: move in discrete steps, emitting a position change per step
    // Move towards targetPos, consuming remainingSpeed (in feet), stopping within stopDistance.
    // stepFeet controls the granularity of events (default 5 ft).
    void MoveTowardsInSteps(Monster& mover,
                            const Position& targetPos,
                            double& remainingSpeed,
                            double stopDistance,
                            double stepFeet = 5.0);

    // Move away from fromPos until at least targetDistance away, consuming remainingSpeed.
    // stepFeet controls the granularity (default 5 ft).
    void MoveAwayInSteps(Monster& mover,
                         const Position& fromPos,
                         double& remainingSpeed,
                         double targetDistance,
                         double stepFeet = 5.0);
private:
    int m_width;
    int m_height;
};

} // namespace itsamonster
