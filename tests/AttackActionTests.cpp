#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "actions/AttackAction.hpp"
#include "monsters/Larvae.hpp"
#include "Dice.hpp"
#include "test_utils/MockDice.hpp"

using namespace itsamonster;

struct TestMonster : public Monster {
    TestMonster() : Monster("TestMonster", 50, 10, 30, {
        std::make_pair(10,0), std::make_pair(10,0), std::make_pair(10,0),
        std::make_pair(10,0), std::make_pair(10,0), std::make_pair(10,0) }) {}
    void TakeAction(Monster& target) override { /* unused */ }
};

TEST(AttackActionTest, HitAndMissControlledByDice) {
    TestMonster attacker;
    TestMonster target;
    AttackAction attack("TestAttack", 0, 5, DamageType::Slashing, 100);

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
