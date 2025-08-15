#pragma once

#include "Logger.hpp"
#include "Types.hpp"
#include "Battlefield.hpp"

#include <array>
#include <string>
#include <utility>
#include <random>
#include "Dice.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace itsamonster {

using ConditionTracker = std::array<int, static_cast<size_t>(Condition::Count)>; // Track conditions using bitmask

struct RoundTracker {
    int rounds = 0;
    bool reaction = false;
    bool bonusAction = false;
};

class Monster {
public:
    Monster(std::string_view n, int h, int a, int spd, std::array<std::pair<int, int>, 6> s)
        : m_name(std::move(n)), m_hp(h), m_ac(a), m_speed(spd), m_stats(std::move(s)) {}
    virtual ~Monster() = default;

        // Movement & spatial
    virtual int GetSpeed() const { return m_speed; }
    virtual Position GetPosition() const { return m_position; }
    virtual void SetPosition(Position p) { m_position = p; }
    // Move towards a target position by at most remaining speed (simple straight-line for now)
    virtual void MoveTowards(const Monster& target, double &remainingSpeed);
    virtual void ResetMovementBudget() { m_movementBudget = static_cast<double>(m_speed); }
    virtual double GetRemainingMovement() const { return m_movementBudget; }

    virtual std::string_view GetName() const { return m_name; }
    virtual int GetHP() const { return m_hp; }
    virtual int GetAC() const { return m_ac; }

    virtual bool IsCondition(Condition condition) const;
    virtual void SetCondition(Condition condition, int duration);
    virtual bool SavingThrow(Stat stat, int DC);
    virtual void TakeAction(Monster& target) = 0;
    virtual void TakeDamage(DamageType type, int damage);
    virtual void TakeReaction(Monster& attacker, int damage, bool ishit) {}
    virtual void StartTurn(int round);
    virtual void EndTurn();

    virtual bool IsImmune(DamageType type) const { return false; }
    virtual bool IsVulnerable(DamageType type) const { return false; }
    virtual bool IsResistant(DamageType type) const { return false; }
    virtual bool HasDarkvision() const { return false; }
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
    double m_movementBudget{ 0.0 };
protected:
    RoundTracker m_round{};
};

} // namespace itsamonster
