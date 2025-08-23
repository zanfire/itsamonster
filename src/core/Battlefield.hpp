#pragma once

#include "Types.hpp"
#include "TurnEvent.hpp"

#include <cmath>
#include <map>
#include <optional>

namespace itsamonster {

class CombatSystem;



class Monster; // forward declaration

class Battlefield : public TurnEventListener {
public:
    Battlefield(CombatSystem& system, int w, int h) : m_system(system), m_width(w), m_height(h) {}

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    Position Clamp(Position p) const {
        //p.x = p.x / 5.0 * 5.0f + 2.5f;
        //p.y = p.y / 5.0 * 5.0f + 2.5f;

        if (p.x < 0) p.x = 0; if (p.x >= m_width) p.x = m_width - 1;
        if (p.y < 0) p.y = 0; if (p.y >= m_height) p.y = m_height - 1;
        return p;
    }

    // Movement helpers: move in discrete steps, emitting a position change per step
    // Move towards targetPos, consuming remainingSpeed (in feet), stopping within stopDistance.
    // stepFeet controls the granularity of events (default 5 ft).
    double MoveTowardsInSteps(Monster& mover, const Position& targetPos, double& remainingSpeed, double stopDistance, double stepFeet = 5.0);

    // Move away from fromPos until at least targetDistance away, consuming remainingSpeed.
    // stepFeet controls the granularity (default 5 ft).
    void MoveAwayInSteps(Monster& mover, const Position& fromPos, double& remainingSpeed, double targetDistance, double stepFeet = 5.0);

    double GetDistance(MonsterInstanceId a, MonsterInstanceId b) const;

    void SetPosition(Monster& monster, Position p);
    std::optional<Position> GetPosition(MonsterInstanceId monster) const;

    void SetDarkness(bool darkness);
    bool OnTurnEvent(TurnEvent ev, EventPayload* payload);

    void ShowMap() const;
private:
    CombatSystem& m_system;
    std::map<MonsterInstanceId, Position> m_positions; // Track monster positions
    int m_width;
    int m_height;
    bool m_darkness{ false }; // Whether the battlefield is in darkness, affecting visibility
};

} // namespace itsamonster
