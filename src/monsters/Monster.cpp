#include "Monster.hpp"
#include <algorithm>

using namespace itsamonster;

bool Monster::IsCondition(Condition condition) const {
    return m_conditions[int(condition)] > 0;
}

bool Monster::SavingThrow(Ability stat, int DC) {
    int result = GetDice().Roll(20) + m_stats[int(stat)].second;
    if (result >= DC) {
        LOG(m_name << " succeeds the saving throw against " << to_string(stat));
        return true;
    } else {
        LOG(m_name << " fails the saving throw against " << to_string(stat));
    }
    return false;
}

void Monster::SetCondition(Condition condition, int deadline) {
    m_conditions[int(condition)] = deadline;
    LOG(m_name << " is set to " << to_string(condition) << " until round " << deadline << " is round " << m_round.rounds);
}

void Monster::TakeDamage(DamageType type, int damage) {
    if (IsImmune(type)) {
        LOG(m_name << " is immune to " << to_string(type) << ", no damage taken.");
        return;
    }
    if (IsResistant(type)) {
        damage /= 2;
        LOG(m_name << " is resistant to " << to_string(type) << ", damage halved to " << damage);
    }
    if (IsVulnerable(type)) {
        damage *= 2;
        LOG(m_name << " is vulnerable to " << to_string(type) << ", damage doubled to " << damage);
    }
    int before = m_hp;
    m_hp -= damage;
    LOG("    " << m_name << " takes " << damage << " damage (" << before << " -> " << m_hp << ") " << to_string(type) << "\n");
}

void Monster::StartTurn(int round)  {
    m_round = {};
    m_round.rounds = round;
}

void Monster::EndTurn() {
    int condition = 0;
    for (auto &deadline : m_conditions) {
        if (deadline <= m_round.rounds && deadline != 0) {
            LOG(m_name << " condition " << to_string(static_cast<Condition>(condition)) << " has ended.");
            deadline = 0; // Remove expired condition
        }
        ++condition;
    }
}

void Monster::MoveTowards(const Monster& target, double &remainingSpeed) {
    if (remainingSpeed <= 0) return;
    Position tp = target.GetPosition();
    double dist = m_position.DistanceTo(tp);
    if (dist <= 5.0) return; // already adjacent (within 5ft)
    double move = std::min(dist - 5.0, remainingSpeed); // stop at 5ft distance
    if (move <= 0) return;
    // simple linear interpolation
    double ratio = move / dist;
    m_position.x = static_cast<int>(std::round(m_position.x + (tp.x - m_position.x) * ratio));
    m_position.y = static_cast<int>(std::round(m_position.y + (tp.y - m_position.y) * ratio));
    remainingSpeed -= move;
}