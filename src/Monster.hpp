#pragma once

#include "Logger.hpp"
#include "Types.hpp"

#include <array>
#include <string>
#include <utility>
#include <random>
#include <iostream>

namespace itsamonster {

struct ConditionTracker {
    //
    std::array<int, static_cast<size_t>(Condition::Count)> conditionsRounds{}; // Track conditions using bitmask
};

struct RoundTracker {
    int rounds = 0;
    bool reaction = false;
    bool bonusAction = false;
};

class Monster {
public:
    Monster(std::string_view n, int h, int a, std::array<std::pair<int, int>, 6> s)
        : m_name(std::move(n)), m_hp(h), m_ac(a), m_stats(std::move(s)) {}
    virtual ~Monster() = default;

    virtual std::string_view GetName() const { return m_name; }
    virtual int GetHP() const { return m_hp; }
    virtual int GetAC() const { return m_ac; }

    virtual bool IsCondition(Condition condition) const;
    virtual bool SavingThrow(Stat stat, int DC, std::mt19937 &rng);
    virtual void SetCondition(Condition condition, int duration);
    virtual void TakeAction(Monster &target, std::mt19937 &rng) = 0;
    virtual void TakeDamage(DamageType type, int damage,  std::mt19937 &rng);
    virtual void TakeReaction(Monster &attacker, int damage, bool ishit, std::mt19937 &rng) {}
    virtual void StartTurn(int round, std::mt19937 &rng);
    virtual void EndTurn(std::mt19937 &rng);
private:
    std::string_view m_name;
    int m_hp;
    int m_ac;
    std::array<std::pair<int, int>, 6> m_stats{};
    ConditionTracker m_conditions;
protected:
    RoundTracker m_round;
};

} // namespace itsamonster
