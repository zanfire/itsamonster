#pragma once

#include <optional>
#include <string_view> // added

#include "Types.hpp"

namespace itsamonster {

struct Monster;
using MonsterPtr = Monster*;
struct Reaction;

/// @brief Event types for turn-based actions in the game.
/// These events are used to track the state of a monster's turn and notify listeners.
enum class TurnEvent {
    MonsterEnter, // New monster added
    MonsterDie,
    MonsterLeave, // Monster removed
    NewRound,
    StartTurn,
    EndTurn,
    BeforeMove,
    AfterMove,
    EnteringReach,
    LeavingReach,
    TakeAction,
    TakeBonusAction,
    TakeReaction,
    BeforeAction,
    AfterAction,
    AttackRoll,
    OnHit,
    OnMiss,
    OnDamageApplied,
    OnApplyCondition,
    OnRemoveCondition,
    OnReactionCompleted
};

/// Convert TurnEvent to a human-readable name.
inline constexpr std::string_view to_string(TurnEvent ev) noexcept {
    switch (ev) {
        case TurnEvent::MonsterEnter:      return "MonsterEnter";
        case TurnEvent::MonsterDie:         return "MonsterDie";
        case TurnEvent::MonsterLeave:       return "MonsterLeave";
        case TurnEvent::NewRound:           return "NewRound";
        case TurnEvent::StartTurn:          return "StartTurn";
        case TurnEvent::EndTurn:            return "EndTurn";
        case TurnEvent::BeforeMove:         return "BeforeMove";
        case TurnEvent::AfterMove:          return "AfterMove";
        case TurnEvent::EnteringReach:      return "EnteringReach";
        case TurnEvent::LeavingReach:       return "LeavingReach";
        case TurnEvent::TakeAction:         return "TakeAction";
        case TurnEvent::TakeBonusAction:    return "TakeBonusAction";
        case TurnEvent::TakeReaction:       return "TakeReaction";
        case TurnEvent::BeforeAction:       return "BeforeAction";
        case TurnEvent::AfterAction:        return "AfterAction";
        case TurnEvent::AttackRoll:         return "AttackRoll";
        case TurnEvent::OnHit:              return "OnHit";
        case TurnEvent::OnMiss:             return "OnMiss";
        case TurnEvent::OnDamageApplied:    return "OnDamageApplied";
        case TurnEvent::OnApplyCondition:   return "OnApplyCondition";
        case TurnEvent::OnRemoveCondition:  return "OnRemoveCondition";
        case TurnEvent::OnReactionCompleted:return "OnReactionCompleted";
    }
    return "UnknownTurnEvent";
}

enum class Phase { Before, After };

/// @brief Base class for event payloads passed through OnTurnEvent.
/// Listeners can downcast to specific payload types and modify them in-place.
struct EventPayload {
    virtual ~EventPayload() = default;
};

struct NewRoundPayload : public EventPayload {
    int round{ 0 };                // New round
};

struct MonsterPayload : public EventPayload {
    MonsterPtr monster{ nullptr }; // Pointer to the monster entering
};

struct AttackRollPayload : public MonsterPayload {
    Monster* target{ nullptr }; // Target monster for the attack
    Advantage advantage{ Advantage::Normal }; // Advantage/Disadvantage state
    int attackRoll{ 0 }; // Result of the attack roll
    int ac{ 0 }; // Target's AC for the attack
    Phase phase{ Phase::Before }; // Phase of the attack roll

};

/// @brief Payload describing damage about to be applied or just applied.
enum class DamagePhase { BeforeApply, AfterApply };
struct DamagePayload : public MonsterPayload {
    std::vector<std::pair<DamageType, int>> damages{}; // List of damage types and amounts
    bool critical{ false };      // Optional flag for crit handling
    DamagePhase phase{ DamagePhase::BeforeApply };
};

/// @brief Payload describing a position change intent or result.
struct PositionPayload : public MonsterPayload {
    std::optional<Position> oldPos{}; // May be empty for teleports/spawns
    Position newPos{};                // Modifiable for effects that alter movement
};

/// @brief Payload describing a condition application/removal.
struct ConditionPayload : public MonsterPayload {
    std::optional<Condition> condition{}; // Which condition to apply/remove
    int duration{ 0 }; // Rounds; modifiable by listeners
};

struct MonsterEnterPayload : public MonsterPayload {
    Position spawnPos{};           // Position where the monster spawns
    int round{ 0 };                // Round when the monster enters
    int initiative{ 0 };           // Initiative value for sorting
    int faction{ 0 };             // Faction ID for team identification
};

struct ConditionEventPayload : public MonsterPayload {
    Condition condition{};         // Condition being applied or removed
    int duration{ 0 };             // Duration in rounds, modifiable by listeners
    Phase phase{ Phase::Before };  // Phase of the condition event
};

struct TurnEventListener {
    virtual ~TurnEventListener() = default;
    virtual bool OnPositionChanged(Monster& monster, std::optional<Position> oldPos, Position newPos) { return true; }
    /// @brief Generic turn event with an optional, modifiable payload.
    /// Listeners can inspect and mutate the payload via dynamic_cast to known types
    /// such as DamagePayload, PositionPayload, ConditionPayload, etc.
    /// Returning false can be used to signal cancellation if the event supports it.
    virtual bool OnTurnEvent(TurnEvent ev, EventPayload* payload) { (void)payload; return true; }
};

} // namespace itsamonster
