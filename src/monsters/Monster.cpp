#include "Monster.hpp"

#include "core/CombatSystem.hpp"
#include "core/TurnTracker.hpp"
#include <iomanip>

using namespace itsamonster;

// Initialize the global atomic instance ID counter (starts at 0 so first assigned is 1)
std::atomic<MonsterInstanceId> Monster::s_nextId{ 0 };

Monster::~Monster() = default;

bool Monster::IsCondition(Condition condition) const {
    if (auto status = m_system.GetTurnStatusTracker().GetTurnStatus(GetInstanceId())) {
        // Check if the condition is active
        return status->conditions[int(condition)] > 0;
    }
    return false;
}
void Monster::SetCondition(Condition condition, int duration) {
    if (duration <= 0) {
        LOGGER.LogMonster(*this, "tried to set condition %s with non-positive duration, ignoring.", to_string(condition).data());
        return; // Invalid duration
    }
    ConditionEventPayload payload{};
    payload.monster = this;
    payload.condition = condition;
    payload.phase = Phase::Before;
    payload.duration = duration;
    if (m_system.NotifyTurnEvent(TurnEvent::OnApplyCondition, &payload)) {
        payload.phase = Phase::After;
        // Only apply condition if the Before phase was accepted
        m_system.NotifyTurnEvent(TurnEvent::OnApplyCondition, &payload);
        LOGGER.LogMonster(*this, "applied condition %s for %d rounds.", to_string(condition).data(), duration);
    } else {
        LOGGER.LogMonster(*this, "failed to apply condition %s, cancelled by listener.", to_string(condition).data());
        return; // Cancelled by listener
    }
}

bool Monster::SavingThrow(Ability stat, int DC) {
    Advantage adv = Advantage::Normal;
    if (stat == Ability::Dexterity && IsCondition(Condition::Restrained)) {
        LOGGER.LogMonster(*this, "is restrained, saving throw with disadvantage.");
        adv = Advantage::Disadvantage;
    }
    int result = GetDice().D20(adv) + m_stats[int(stat)].second;
    if (result >= DC) {
        LOGGER.LogMonster(*this, "succeeds the saving throw against %s", to_string(stat).data());
        return true;
    } else {
        LOGGER.LogMonster(*this, "fails the saving throw against %s", to_string(stat).data());
    }
    return false;
}

void Monster::TakeDamage(DamageType type, int damage) {}

bool Monster::OnPositionChanged(Monster& monster, std::optional<Position> oldPos, Position newPos) {
    if (&monster != this) return true;
    std::cout << std::fixed << std::setprecision(2);

    LOGGER.LogMonster(monster, "Moved from %d,%d to %d,%d", (oldPos ? oldPos->x : 0), (oldPos ? oldPos->y : 0), newPos.x, newPos.y);
    return true;
}

bool Monster::OnTurnEvent(TurnEvent ev, EventPayload* payload) {
    auto monsterPayload = dynamic_cast<MonsterPayload*>(payload);
    if (monsterPayload == nullptr || monsterPayload->monster != this) return true;

    switch (ev) {
    case TurnEvent::StartTurn:
        LOGGER.LogMonster(*monsterPayload->monster, "starts their turn.");
        break;
    case TurnEvent::EndTurn:
        LOGGER.LogMonster(*monsterPayload->monster, "ends their turn.");
        break;
    case TurnEvent::BeforeAction:
        LOGGER.LogMonster(*monsterPayload->monster, "is about to act.");
        break;
    case TurnEvent::AfterAction:
        LOGGER.LogMonster(*monsterPayload->monster, "has completed their action.");
        break;
    case TurnEvent::OnDamageApplied:
    {
        auto dmgPayload = dynamic_cast<DamagePayload*>(payload);
        if (dmgPayload) {
            return OnDamageApplied(dmgPayload);
        }
        break;
    }
    case TurnEvent::OnApplyCondition:
        return OnApplyCondition(dynamic_cast<ConditionEventPayload*>(payload));
        break;
    }
    return true;
}

bool Monster::OnApplyCondition(ConditionEventPayload* payload) {
    if (payload == nullptr) {
        LOGGER.LogMonster(*this, "received null condition payload, ignoring.");
        return true; // Nothing to do
    }
    if (payload->monster != this) return true; // Not our condition
    if (payload->phase == Phase::Before) {
        LOGGER.LogMonster(*this, "is about to apply condition %s for %d rounds.", to_string(payload->condition).data(), payload->duration);
        if (IsImmune(payload->condition)) {
            LOGGER.LogMonster(*this, "is immune to %s, condition not applied.", to_string(payload->condition).data());
            return false; // Skip applying condition
        }
    }
    return true;
}

bool Monster::OnDamageApplied(DamagePayload* payload) {
    if (payload->monster != this) return true; // Not our damage
    if (payload->phase != DamagePhase::AfterApply) {;
        return true; // Only handle after-apply phase
    }
    for (auto& [type, amount] : payload->damages) {
        if (IsImmune(type)) {
            amount = 0;
            LOGGER.LogMonster(*this, "is immune to %s, no damage taken.", to_string(type).data());
        }
        if (IsResistant(type)) {
            amount /= 2;
            LOGGER.LogMonster(*this, "is resistant to %s, damage halved to %d.", to_string(type).data(), amount);
        }
        if (IsVulnerable(type)) {
            amount *= 2;
            LOGGER.LogMonster(*this, "is vulnerable to %s, damage doubled to %d.", to_string(type).data(), amount);
        }
    }
    return true;
}