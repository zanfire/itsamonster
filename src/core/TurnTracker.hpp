#pragma once

#include "TurnEvent.hpp"

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

/// @brief Tracks the state of a monster's turn.
struct TurnStatus {
    int round{ 0 };
    int initiative{ 0 };
    int damageTaken{ 0 };
    bool dead{ false };
    /// @brief Pointer (not owning) to the monster whose turn is being tracked.
    Monster* self{ nullptr };
    TurnAction actions{};
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
    TurnStatus* GetTurnStatus(MonsterInstanceId id) {
        auto it = m_turnTrackers.find(id);
        if (it != m_turnTrackers.end()) {
            return &it->second;
        }
        return nullptr;
    }
private:
    void AddMonster(MonsterEnterPayload* payload);
    bool TrackDamage(DamagePayload* payload);
private:
    std::map<MonsterInstanceId, TurnStatus> m_turnTrackers;
    std::vector<MonsterPtr> m_turnOrder; // Sorted by initiative
    class CombatSystem& m_combatSystem;
};
} // namespace itsamonster
