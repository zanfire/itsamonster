#pragma once

#include "Logger.hpp"
#include "Types.hpp"
#include "core/Dice.hpp"
#include "core/Battlefield.hpp"
#include "core/CombatSystem.hpp"
#include "actions/DashAction.hpp"
#include "reactions/Reaction.hpp"

#include <array>
#include <string>
#include <utility>
#include <random>
#include <iostream>
#include <memory>
#include <vector>
#include <atomic>

namespace itsamonster {

using MonsterPtr = Monster*;

struct RoundTracker {
    int rounds{ 0 };
    bool reaction{ false };
    bool bonusAction{ false };
    int movement{ 0 };
};

struct Strategy; // forward declaration for unique_ptr member

class Monster : public TurnEventListener {
public:
    Monster(CombatSystem& system, std::string_view name, int hp, int ac, int speed, std::array<std::pair<int, int>, 6> s)
        : m_name(std::move(name)), m_hp(hp), m_ac(ac), m_speed(speed), m_stats(std::move(s)), m_dash(system), m_system(system) {
        // Assign a unique, monotonically increasing instance ID starting from 1
        m_id = s_nextId.fetch_add(1, std::memory_order_relaxed) + 1;
    }
    virtual ~Monster();

    /// @brief Return the unique instance ID of the monster.
    /// @return The unique instance ID.
    MonsterInstanceId GetInstanceId() const { return m_id; }

    // Movement & spatial
    virtual int GetSpeed() const { return m_speed; }

    virtual std::string_view GetName() const { return m_name; }
    virtual int GetHP() const { return m_hp; }
    virtual int GetAC() const { return m_ac; }

    virtual int GetReach() const { return 5; } // Default reach for melee attacks

    virtual struct AttackMeleeAction* GetMeleeAttack() { return nullptr; }
    virtual struct AttackRangedAction* GetRangedAttack() { return nullptr; }
    virtual struct DashAction* GetDashAction() { return &m_dash; }

    virtual bool IsCondition(Condition condition) const;
    virtual void SetCondition(Condition condition, int duration);
    virtual bool SavingThrow(Ability stat, int DC);
    // Strategy hook: by default, CombatSystem will call the AI strategy if one is attached.
    // Monsters can still override TakeAction to define their attacks.
    virtual void TakeDamage(DamageType type, int damage);
    virtual void TakeReaction(Monster& attacker, int damage, bool ishit) {}

    virtual bool IsImmune(Condition condition) const { return false; }
    virtual bool IsImmune(DamageType type) const { return false; }
    virtual bool IsVulnerable(DamageType type) const { return false; }
    virtual bool IsResistant(DamageType type) const { return false; }
    virtual bool HasDarkvision() const { return false; }
    virtual MonsterSize GetMonsterSize() const { return MonsterSize::Medium; }

    // AI strategy accessors (non-owning pointer; external code manages lifetime)
    void SetAI(std::shared_ptr<struct Strategy> ai) { m_ai = ai; }
    struct Strategy* GetAI() const { return m_ai.get(); }
private:
    bool OnPositionChanged(Monster& monster, std::optional<Position> oldPos, Position newPos) override;
    bool OnTurnEvent(TurnEvent ev, EventPayload* payload) override;

    bool OnApplyCondition(ConditionEventPayload* payload);
    bool OnDamageApplied(DamagePayload* payload);
private:
    std::string_view m_name;
    MonsterInstanceId m_id{0};
    int m_hp{ 0 };
    int m_ac{ 0 };
    int m_speed{ 0 }; // feet per round
    int m_flySpeed{ 0 }; // feet per round, if applicable
    bool m_hover{ false }; // true if the monster can hover (e.g. flying creatures)
    std::array<std::pair<int, int>, 6> m_stats{};
    DashAction m_dash;
protected:
    std::shared_ptr<struct Strategy> m_ai{};
    CombatSystem& m_system;

private:
    // Global, process-wide counter for generating unique Monster instance IDs.
    static std::atomic<MonsterInstanceId> s_nextId;
};

} // namespace itsamonster
