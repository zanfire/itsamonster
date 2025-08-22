#include "Action.hpp"
#include "Logger.hpp"
#include "Types.hpp"

using namespace itsamonster;

void Action::Perform(Monster& attacker, Monster& target) {
    Execute(attacker, target);
}
