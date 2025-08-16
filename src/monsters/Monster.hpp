#pragma once

#include "Logger.hpp"
#include "Types.hpp"
#include "core/Dice.hpp"
#include "core/Battlefield.hpp"

#include <array>
#include <string>
#include <utility>
#include <random>
#include <iostream>
#include <memory>
#include <vector>

namespace itsamonster {

using ConditionTracker = std::array<int, static_cast<size_t>(Condition::Count)>; // Track conditions using bitmask

struct RoundTracker {
    int rounds{ 0 };
    bool reaction{ false };
    bool bonusAction{ false };
    int movement{ 0 };
};

struct IAIStrategy; // forward declaration for unique_ptr member

class Monster {
public:
    Monster(std::string_view n, int h, int a, int spd, std::array<std::pair<int, int>, 6> s)
        : m_name(std::move(n)), m_hp(h), m_ac(a), m_speed(spd), m_stats(std::move(s)) {}
    virtual ~Monster();

        // Movement & spatial
    virtual int GetSpeed() const { return m_speed; }
    virtual Position GetPosition() const { return m_position; }
    virtual void SetPosition(Position p);

    virtual std::string_view GetName() const { return m_name; }
    virtual int GetHP() const { return m_hp; }
    virtual int GetAC() const { return m_ac; }

    virtual int GetReach() const { return 5; } // Default reach for melee attacks

    virtual bool IsCondition(Condition condition) const;
    virtual void SetCondition(Condition condition, int duration);
    virtual bool SavingThrow(Ability stat, int DC);
    virtual void TakeAction(Monster& target) = 0;
    // Strategy hook: by default, CombatSystem will call the AI strategy if one is attached.
    // Monsters can still override TakeAction to define their attacks.
    virtual void TakeDamage(DamageType type, int damage);
    virtual void TakeReaction(Monster& attacker, int damage, bool ishit) {}
    virtual void StartTurn(int round);
    virtual void EndTurn();

    virtual bool IsImmune(DamageType type) const { return false; }
    virtual bool IsVulnerable(DamageType type) const { return false; }
    virtual bool IsResistant(DamageType type) const { return false; }
    virtual bool HasDarkvision() const { return false; }

    // AI strategy accessors (non-owning pointer; external code manages lifetime)
    void SetAI(struct IAIStrategy* ai) { m_ai = ai; }
    struct IAIStrategy* GetAI() const { return m_ai; }
private:
    std::string_view m_name;
    int m_hp{ 0 };
    int m_ac{ 0 };
    int m_speed{ 0 }; // feet per round
    int m_flySpeed{ 0 }; // feet per round, if applicable
    bool m_hover{ false }; // true if the monster can hover (e.g. flying creatures)
    std::array<std::pair<int, int>, 6> m_stats{};
    ConditionTracker m_conditions{};
    Position m_position{};
protected:
    RoundTracker m_round{};
    struct IAIStrategy* m_ai{ nullptr };
};

} // namespace itsamonster
