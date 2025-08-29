#include "actions/RechargeAction.hpp"
#include "Logger.hpp"
#include "monsters/Monster.hpp"

using namespace itsamonster;

bool RechargeAction::OnTurnEvent(TurnEvent ev, EventPayload* p) {
    if (ev == TurnEvent::StartTurn) {
        auto payload = dynamic_cast<MonsterPayload*>(p);
        if (payload == nullptr || payload->monster != m_self) return true;
        TryRecharge();
    }
    return true;
}

void RechargeAction::TryRecharge() {
    if (m_available) return;
    int roll = GetDice().Roll(6);
    if (roll >= m_rechargeMin) {
        m_available = true;
        LOGGER.LogMonster(*m_self, "recharges on a %d (needed %d+)", roll, m_rechargeMin);
    }
}

bool RechargeAction::IsInRange(const Monster& attacker, const Monster& target) const {
    auto battlefield = m_system.GetBattlefield();
    double distance = battlefield.GetDistance(attacker.GetInstanceId(), target.GetInstanceId());
    return distance <= m_range;
}

void RechargeAction::RaiseDamage(Monster& attacker, Monster& target, std::vector<std::pair<DamageType, int>> damages) {
    DamagePayload damagePayload{};
    damagePayload.monster = &target;
    damagePayload.damages = std::move(damages);
    damagePayload.phase = DamagePhase::BeforeApply;
    if (m_system.NotifyTurnEvent(TurnEvent::OnDamageApplied, &damagePayload)) {
        damagePayload.phase = DamagePhase::AfterApply;
        m_system.NotifyTurnEvent(TurnEvent::OnDamageApplied, &damagePayload);
    }
    else {
        LOGGER.LogMonster(attacker, "Hit cancelled by OnDamageApplied listener.");
        return; // Cancelled by listener
    }
}