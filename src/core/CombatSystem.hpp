#pragma once

#include <string_view>
#include <optional>
#include <cstdint>
#include <memory>
#include <vector>
#include <array>

#include "Types.hpp"
#include "monsters/Monster.hpp"
#include "Battlefield.hpp"

namespace itsamonster {

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
    std::vector<Position> path{};
    bool usedReaction{ false };
    bool usedBonusAction{ false };
    bool usedAction{ false };
};

/// @brief Tracks the state of a monster's turn.
struct TurnTracker {
    int round{ 0 };
    /// @brief Pointer (not owning) to the monster whose turn is being tracked.
    Monster* self{ nullptr };
    Resource resources{};
};

struct TurnEventListener {
    virtual ~TurnEventListener() = default;
    virtual void OnPositionChanged(Monster& monster, const Position& oldPos, const Position& newPos) {}
    virtual void OnTurnEvent(Monster& monster, TurnEvent ev, TurnTracker& ctx) {}
};

/// @brief Manages the turn-based system for monsters.
/// @remark Singleton per thread; not thread-safe across threads.
class CombatSystem {
public:
    // Per-thread singleton accessor
    static CombatSystem& Instance() {
        thread_local CombatSystem instance;
        return instance;
    }

    // Non-copyable, non-movable
    CombatSystem(const CombatSystem&) = delete;
    CombatSystem& operator=(const CombatSystem&) = delete;
    CombatSystem(CombatSystem&&) = delete;
    CombatSystem& operator=(CombatSystem&&) = delete;

    void AddListener(TurnEventListener* listener);
    void RemoveListener(TurnEventListener* listener);
    void NotifyPositionChanged(Monster& monster, const Position& oldPos, const Position& newPos);
    void NotifyTurnEvent(Monster& monster, TurnEvent ev, TurnTracker& ctx);

    void Round(std::array<Monster*, 2> initiativeOrder);

    // Trigger a reaction owned by `self` in response to `other`
    void TriggerReaction(Monster& self, Monster& other, Reaction& reaction);

    // Test/inspection helper: check if a monster has used its reaction in current round
    bool HasUsedReaction(const Monster& monster) const;

    int GetCurrentRound() const { return m_round;}

private:
    CombatSystem() = default;
    ~CombatSystem() = default;

    TurnTracker* GetTurnTracker(Monster& monster);
    const TurnTracker* GetTurnTracker(const Monster& monster) const;

private:
    std::vector<TurnEventListener*> m_listeners;
    std::array<TurnTracker, 2> m_turnTrackers;
    int m_round{ 0 };
};
} // namespace itsamonster
