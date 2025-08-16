#include "Monster.hpp"
#include <algorithm>
#include "core/CombatSystem.hpp"

using namespace itsamonster;

Monster::~Monster() = default;
void Monster::SetPosition(Position p) {
    Position old = m_position;
    m_position = p;
    CombatSystem::Instance().NotifyPositionChanged(*this, old, m_position);
}

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
