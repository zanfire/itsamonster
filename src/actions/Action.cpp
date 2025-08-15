#include "Action.hpp"
#include "Logger.hpp"
#include "Types.hpp"

using namespace itsamonster;

void Action::Perform(Monster& attacker, Monster& target) {
    if (!CanAttack(attacker, target)) {
        LOG(attacker.GetName() << " cannot attack " << target.GetName() << "!");
        return;
    }
    Execute(attacker, target);
}

bool Action::CanAttack(const Monster& attacker, const Monster& target) const {
    if (attacker.IsCondition(Condition::Incapacitated)) {
        LOG(attacker.GetName() << " is incapacitated and cannot take actions!");
        return false;
    }
    if (!IsInRange(attacker, target)) {
        LOG(attacker.GetName() << " is out of range for this action");
        return false; // can't attack this action
    }
    return true;
}
