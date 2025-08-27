#pragma once

#include "Types.hpp"
#include "TurnEvent.hpp"

#include <array>
#include <map>
#include <vector>

namespace itsamonster {

struct Monster;
using MonsterPtr = Monster*;
struct Reaction;

struct TurnAction {
    bool reaction{ false };
    bool bonusAction{ false };
    bool action{ false };
    double movement{ 0 }; // Movement used this turn
};

using ConditionTracker = std::array<int, static_cast<size_t>(Condition::Count)>; // Track conditions using bitmask

/// @brief Tracks the state of a monster's turn.
struct TurnStatus {
    int round{ 0 };
    int initiative{ 0 };
    int damageTaken{ 0 };
    bool dead{ false };
    int faction{ 0 }; // Faction ID for team identification
    /// @brief Pointer (not owning) to the monster whose turn is being tracked.
    Monster* self{ nullptr };
    TurnAction actions{};
    ConditionTracker conditions{};

};

/// @brief Manages the turn-based system for monsters.
/// @remark Singleton per thread; not thread-safe across threads.
class TurnStatusTracker : public TurnEventListener {
public:
    TurnStatusTracker(class CombatSystem& combatSystem);
    ~TurnStatusTracker();

    // Non-copyable, non-movable
    TurnStatusTracker(const TurnStatusTracker&) = delete;
    TurnStatusTracker& operator=(const TurnStatusTracker&) = delete;
    TurnStatusTracker(TurnStatusTracker&&) = delete;
    TurnStatusTracker& operator=(TurnStatusTracker&&) = delete;

    bool OnTurnEvent(TurnEvent ev, EventPayload* payload) override;

    const std::vector<MonsterPtr>& GetTurnOrder() const { return m_turnOrder; }
    const TurnStatus* GetTurnStatus(MonsterInstanceId id) const {
        auto it = m_turnTrackers.find(id);
        if (it != m_turnTrackers.end()) {
            return &it->second;
        }
        return nullptr;
    }
    TurnStatus* GetTurnStatus(MonsterInstanceId id) {
        auto it = m_turnTrackers.find(id);
        if (it != m_turnTrackers.end()) {
            return &it->second;
        }
        return nullptr;
    }


    bool IsAlliesNear(MonsterInstanceId id1, MonsterInstanceId id2, int range) const;
private:
    void AddMonster(MonsterEnterPayload* payload);
    bool TrackDamage(DamagePayload* payload);
    bool TrackCondition(ConditionEventPayload* payload);
    bool TrackStartTurn(MonsterPayload* payload);
    bool TrackEndTurn(MonsterPayload* payload);
private:
    std::map<MonsterInstanceId, TurnStatus> m_turnTrackers;
    std::vector<MonsterPtr> m_turnOrder; // Sorted by initiative
    class CombatSystem& m_system;
};
} // namespace itsamonster
