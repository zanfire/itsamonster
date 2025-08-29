#include "actions/DashAction.hpp"

#include "Logger.hpp"
#include "Types.hpp"
#include "monsters/Monster.hpp"
#include "core/Battlefield.hpp"
#include "core/TurnTracker.hpp"

using namespace itsamonster;

void DashAction::Execute(Monster& attacker, Monster& target) {
    if (attacker.IsCondition(Condition::Incapacitated)) {
        LOGGER.LogMonster(attacker, "is incapacitated and cannot Dash!");
        return;
    }

    // Check action economy with TakeAction event
    MonsterPayload mp{};
    mp.monster = &attacker;
    if (!m_system.NotifyTurnEvent(TurnEvent::TakeAction, &mp)) {
        LOGGER.LogMonster(attacker, "Dash action cancelled: action already used or blocked.");
        return;
    }

    auto& battlefield = m_system.GetBattlefield();
    auto& tracker = m_system.GetTurnStatusTracker();
    auto status = tracker.GetTurnStatus(attacker.GetInstanceId());
    if (!status) {
        LOGGER.Debug("Dash failed: attacker not tracked in TurnStatusTracker.");
        return;
    }


    auto spOpt = battlefield.GetPosition(attacker.GetInstanceId());
    auto tpOpt = battlefield.GetPosition(target.GetInstanceId());
    if (!spOpt || !tpOpt) {
        LOGGER.Debug("Dash failed: missing positions.");
        return;
    }

    double remaining = static_cast<double>(attacker.GetSpeed() * 2) - status->actions.movement;
    // Perform the dash movement now, independent of prior movement; action economy already consumed.
    // Use size-aware overload to stop at correct edge-to-edge distance
    double moved = battlefield.MoveTowardsInSteps(attacker, target, remaining, m_stopDistance, 5.0);
    LOGGER.LogMonster(attacker, " dashes %d feet towards %s", moved, target.GetName().data());
}
