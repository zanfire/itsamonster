#pragma once

#include "Logger.hpp"
#include "core/CombatSystem.hpp"

namespace itsamonster {

struct Monster;

enum class TriggerType {
    OnAttack = 1,
    OnDamage,
    OnSavingThrow,
    OnEnterReach,
    OnExitReach
};

struct Reaction : public TurnEventListener {
    Reaction(Monster& monster, std::string_view name) : m_monster(monster), m_name(name) {}
    virtual ~Reaction() = default;

    virtual bool IsTriggered(TriggerType triggerType) const = 0;
    virtual void Trigger(Monster& self, Monster& other, TriggerType triggerType) = 0;

    std::string_view GetName() const { return m_name; }
protected:
    Monster& m_monster;
    std::string_view m_name;
};

struct OpportunityAttackReaction : public Reaction {
    OpportunityAttackReaction(Monster& monster);
    ~OpportunityAttackReaction() override;

    bool IsTriggered(TriggerType triggerType) const override;
    void Trigger(Monster& self, Monster& other, TriggerType triggerType) override;

    void OnPositionChanged(Monster& monster, const Position& oldPos, const Position& newPos);
};

} // namespace itsamonster