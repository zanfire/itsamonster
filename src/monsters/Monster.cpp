#include "Monster.hpp"

#include "core/CombatSystem.hpp"
#include <iomanip>

using namespace itsamonster;

// Initialize the global atomic instance ID counter (starts at 0 so first assigned is 1)
std::atomic<MonsterInstanceId> Monster::s_nextId{ 0 };

Monster::~Monster() = default;

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
    // Allow listeners to modify damage before resistances/vulnerability are applied
    DamagePayload dmg;
    dmg.monster = this;
    dmg.damages.emplace_back(type, damage);
    dmg.phase = DamagePhase::BeforeApply;
    m_system.NotifyTurnEvent(TurnEvent::OnDamageApplied, &dmg);

    if (IsImmune(type)) {
        damage = 0;
        LOG(m_name << " is immune to " << to_string(type) << ", no damage taken.");
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
    auto after = before - damage;
    LOG("    " << m_name << " takes " << damage << " damage (" << before << " -> " << after << ") " << to_string(type) << "\n");
    DamagePayload dmgAfter;
    dmgAfter.monster = this;
    dmgAfter.damages.emplace_back(type, damage);
    dmgAfter.phase = DamagePhase::AfterApply;
    m_system.NotifyTurnEvent(TurnEvent::OnDamageApplied, &dmgAfter);
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

bool Monster::OnPositionChanged(Monster& monster, std::optional<Position> oldPos, Position newPos) {
    if (&monster != this) return true;
    std::cout << std::fixed << std::setprecision(2);
    LOG(monster.GetName() << " moved from " << (oldPos ? std::to_string(oldPos->x) + "," + std::to_string(oldPos->y) : "unknown")
        << " to " << newPos.x << "," << newPos.y);
    return true;
}

bool Monster::OnTurnEvent(TurnEvent ev, EventPayload* payload) {
    auto monsterPayload = dynamic_cast<MonsterPayload*>(payload);
    if (monsterPayload == nullptr || monsterPayload->monster != this) return true;

    switch (ev) {
    case TurnEvent::StartTurn:
        LOG(monsterPayload->monster->GetName() << " starts their turn.");
        break;
    case TurnEvent::EndTurn:
        LOG(monsterPayload->monster->GetName() << " ends their turn.");
        break;
    case TurnEvent::BeforeAction:
        LOG(monsterPayload->monster->GetName() << " is about to act.");
        break;
    case TurnEvent::AfterAction:
        LOG(monsterPayload->monster->GetName() << " has completed their action.");
        break;
    case TurnEvent::OnDamageApplied:
        auto dmgPayload = dynamic_cast<DamagePayload*>(payload);
        if (dmgPayload) {
            OnDamageApplied(dmgPayload);
        }
        break;
    }
    return true;
}

bool Monster::OnDamageApplied(DamagePayload* payload) {
    if (payload->monster != this) return true; // Not our damage
    if (payload->phase != DamagePhase::AfterApply) {
        LOG(m_name << " received damage before application phase, skipping immunity/resistance checks.");
        return true; // Only handle after-apply phase
    }
    for (auto& [type, amount] : payload->damages) {
        if (IsImmune(type)) {
            amount = 0;
            LOG(m_name << " is immune to " << to_string(type) << ", no damage taken.");
        }
        if (IsResistant(type)) {
            amount /= 2;
            LOG(m_name << " is resistant to " << to_string(type) << ", damage halved to " << amount );
        }
        if (IsVulnerable(type)) {
            amount *= 2;
            LOG(m_name << " is vulnerable to " << to_string(type) << ", damage doubled to " << amount);
        }
    }
    return true;
}