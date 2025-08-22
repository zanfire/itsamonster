#pragma once

#include <string_view>
#include <memory>
#include <cstdint>
#include <vector>
#include <map>
#include <optional>

#include "Types.hpp"
#include "core/Battlefield.hpp"
#include "core/TurnEvent.hpp"
#include "core/TurnTracker.hpp"

namespace itsamonster {

struct Monster;
using MonsterPtr = Monster*;
struct Reaction;

/// @brief Manages the turn-based system for monsters.
/// @remark Singleton per thread; not thread-safe across threads.
class CombatSystem {
public:

    CombatSystem(int width = 100, int height = 100)
        : m_battlefield(*this, width, height), m_turnStatusTracker(*this) {
        AddListener(&m_turnStatusTracker);
        AddListener(&m_battlefield);
    }
    ~CombatSystem() {
        RemoveListener(&m_turnStatusTracker);
        RemoveListener(&m_battlefield);
    }

    // Non-copyable, non-movable
    CombatSystem(const CombatSystem&) = delete;
    CombatSystem& operator=(const CombatSystem&) = delete;
    CombatSystem(CombatSystem&&) = delete;
    CombatSystem& operator=(CombatSystem&&) = delete;

    void AddMonster(MonsterPtr monster, int initiative, Position pos, int faction);
    Battlefield& GetBattlefield() { return m_battlefield; }
    TurnStatusTracker& GetTurnStatusTracker() { return m_turnStatusTracker; }

    void AddListener(TurnEventListener* listener);
    void RemoveListener(TurnEventListener* listener);
    void NotifyPositionChanged(Monster& monster, std::optional<Position> oldPos, Position newPos);
    bool NotifyTurnEvent(TurnEvent ev, EventPayload* payload = nullptr);

    /// @brief Run a single round of combat.
    void Round();

    int GetCurrentRound() const { return m_round;}
private:
    void Turn(Monster& monster, std::vector<Monster*> enemies);
private:
    Battlefield m_battlefield; // Default battlefield size, can be adjusted
    TurnStatusTracker m_turnStatusTracker;
    std::vector<TurnEventListener*> m_listeners;
    int m_round{ 0 };
    std::map<MonsterInstanceId, std::vector<Monster*>> m_enemies; // Enemies by monster ID
};
} // namespace itsamonster
