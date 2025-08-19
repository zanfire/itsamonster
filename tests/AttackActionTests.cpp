#include <gtest/gtest.h>

#include "actions/AttackAction.hpp"
#include "core/CombatSystem.hpp"
#include "test_utils/MockDice.hpp"

using namespace itsamonster;

struct TestMonster : public Monster {
    TestMonster(CombatSystem& system) : Monster(system, "TestMonster", 50, 10, 30, {
        std::make_pair(10,0), std::make_pair(10,0), std::make_pair(10,0),
        std::make_pair(10,0), std::make_pair(10,0), std::make_pair(10,0) }) {}
};

TEST(AttackActionTest, HitAndMissControlledByDice) {
    CombatSystem system;
    TestMonster attacker(system);
    TestMonster target(system);
    AttackAction attack(system, "TestAttack", 0, { std::make_pair(DamageType::Slashing, 5) }, 100);

    MockDice mock;
    ScopedDiceOverride scoped(&mock);
    EXPECT_CALL(mock, D20(testing::_))
        .WillOnce(testing::Return(20)) // guaranteed hit (nat 20)
        .WillOnce(testing::Return(1)); // guaranteed miss (nat 1)
    EXPECT_CALL(mock, Roll(testing::_)).Times(testing::AnyNumber());

    int before = target.GetHP();
    attack.Perform(attacker, target); // nat 20 hit
    int mid = target.GetHP();
    attack.Perform(attacker, target); // nat 1 miss
    int after = target.GetHP();

    EXPECT_LT(mid, before); // damage applied on first hit
    EXPECT_EQ(after, mid);  // second attack missed, no further damage
}
