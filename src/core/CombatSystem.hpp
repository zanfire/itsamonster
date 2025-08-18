#pragma once

#include <string_view>
#include <memory>
#include <cstdint>
#include <vector>
#include <map>
#include <optional>

#include "Types.hpp"
#include "core/Battlefield.hpp"

namespace itsamonster {

struct Monster;
using MonsterPtr = Monster*;
struct Reaction;

/// @brief Event types for turn-based actions in the game.
/// These events are used to track the state of a monster's turn and notify listeners.
enum class TurnEvent {
    StartTurn,
    EndTurn,
    BeforeMove,
    AfterMove,
    EnteringReach,
    LeavingReach,
    BeforeAction,
    AfterAction,
    BeforeAttackRoll,
    AfterAttackRoll,
    OnHit,
    OnMiss,
    OnDamageApplied,
    OnApplyCondition,
    OnRemoveCondition,
    OnReactionCompleted
};

struct Resource {
    bool reaction{ false };
    bool bonusAction{ false };
    bool action{ false };
    int movement{ 0 }; // Movement used this turn
};

/// @brief Tracks the state of a monster's turn.
struct TurnTracker {
    int round{ 0 };
    int initiative{ 0 };
    int damageTaken{ 0 };
    /// @brief Pointer (not owning) to the monster whose turn is being tracked.
    Monster* self{ nullptr };
    Resource resources{};
};

struct TurnEventListener {
    virtual ~TurnEventListener() = default;
    virtual void OnPositionChanged(Monster& monster, std::optional<Position> oldPos, Position newPos) {}
    virtual void OnTurnEvent(Monster& monster, TurnEvent ev, TurnTracker& ctx) {}
};

/// @brief Manages the turn-based system for monsters.
/// @remark Singleton per thread; not thread-safe across threads.
class CombatSystem {
public:

    CombatSystem(int width = 100, int height = 100)
        : m_battlefield(*this, width, height) {}
    ~CombatSystem() = default;

    // Non-copyable, non-movable
    CombatSystem(const CombatSystem&) = delete;
    CombatSystem& operator=(const CombatSystem&) = delete;
    CombatSystem(CombatSystem&&) = delete;
    CombatSystem& operator=(CombatSystem&&) = delete;

    void AddMonster(MonsterPtr monster, int initiative, Position pos);
    Battlefield& GetBattlefield() { return m_battlefield; }

    void AddListener(TurnEventListener* listener);
    void RemoveListener(TurnEventListener* listener);
    void NotifyPositionChanged(Monster& monster, std::optional<Position> oldPos, Position newPos);
    void NotifyTurnEvent(Monster& monster, TurnEvent ev, TurnTracker& ctx);

    /// @brief Run a single round of combat.
    void Round();

    // Trigger a reaction owned by `self` in response to `other`
    void TriggerReaction(Monster& self, Monster& other, Reaction& reaction);

    int GetCurrentRound() const { return m_round;}

    TurnTracker* GetTurnTracker(Monster& monster);

private:
    void Turn(Monster& monster, TurnTracker& ctx, std::vector<Monster*> enemies);
private:
    Battlefield m_battlefield; // Default battlefield size, can be adjusted
    std::vector<TurnEventListener*> m_listeners;
    std::map<MonsterInstanceId, TurnTracker> m_turnTrackers;
    int m_round{ 0 };
};
} // namespace itsamonster
