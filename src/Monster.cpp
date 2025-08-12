#include "Monster.hpp"

using namespace itsamonster;

bool Monster::IsCondition(Condition condition) const {
    if (m_conditions.conditionsRounds.find(condition) == m_conditions.conditionsRounds.end()) {
        return false; // Condition not set
    }
    return m_conditions.conditionsRounds.at(condition) > 0; 
}

bool Monster::SavingThrow(Stat stat, int DC, std::mt19937 &rng) {
    std::uniform_int_distribution<int> roll(1, 20);
    int result = roll(rng) + m_stats[stat].second;
    if (result >= DC) {
        LOG(m_name << " succeeds the saving throw against " << to_string(stat));
        return true;
    } else {
        LOG(m_name << " fails the saving throw against " << to_string(stat));
    }
    return false;
}

void Monster::SetCondition(Condition condition, int deadline) {
    m_conditions.conditionsRounds[condition] = deadline;
    LOG(m_name << " is set to " << to_string(condition) << " until round " << deadline << " is round " << m_round.rounds);
}

void Monster::TakeDamage(DamageType type, int damage,  std::mt19937 &rng) {
    int before = m_hp;
    m_hp -= damage;
    LOG("    " << m_name << " takes " << damage << " damage (" << before << " -> " << m_hp << ") " << to_string(type) << "\n");
}

void Monster::StartTurn(int round, std::mt19937 &rng)  {
    m_round = {};
    m_round.rounds = round;
}
    
void Monster::EndTurn(std::mt19937 &rng) {
    for (auto &pair : m_conditions.conditionsRounds) {
        if (pair.second <= m_round.rounds) {
            LOG(m_name << " condition " << to_string(pair.first) << " has ended.");
            pair.second = 0; // Remove expired condition
        }
    }
}