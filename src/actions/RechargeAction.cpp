#include "actions/RechargeAction.hpp"
#include "Logger.hpp"
#include "Types.hpp"

using namespace itsamonster;

void RechargeAction::TryRecharge(std::mt19937& rng) {
    if (m_available) return;
    std::uniform_int_distribution<int> d6(1, 6);
    int roll = d6(rng);
    if (roll >= m_rechargeMin) {
        m_available = true;
        LOG(m_name << " recharges on a " << roll << " (needed " << m_rechargeMin << "+)");
    }
}

bool RechargeAction::IsInRange(const Monster& attacker, const Monster& target) const {
    double distance = attacker.GetPosition().DistanceTo(target.GetPosition());
    return distance <= m_range;
}
