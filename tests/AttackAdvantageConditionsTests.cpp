#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "core/CombatSystem.hpp"
#include "actions/AttackAction.hpp"
#include "monsters/Monster.hpp"
#include "test_utils/MockDice.hpp"

using namespace itsamonster;

struct DummyMonster : public Monster {
    DummyMonster(std::string_view name = "Dummy", int ac = 10)
        : Monster(name, /*hp*/100, /*ac*/ac, /*speed*/30, {
            std::make_pair(10,0), std::make_pair(10,0), std::make_pair(10,0),
            std::make_pair(10,0), std::make_pair(10,0), std::make_pair(10,0) }) {}
    void TakeAction(Monster&) override {}
};

// Helper to build a basic melee and ranged attack
static AttackMeleeAction MakeMelee() {
    return AttackMeleeAction("Melee", /*atk*/0, /*dmg*/1, DamageType::Slashing, /*range*/5);
}
static AttackRangedAction MakeRanged() {
    return AttackRangedAction("Ranged", /*atk*/0, /*dmg*/1, DamageType::Piercing, /*range*/30);
}

// New shared fixture for all tests in this file
class AttackAdvantageTest : public ::testing::Test {
protected:
    DummyMonster a{"A"};
    DummyMonster t{"T"};
    CombatSystem system{100, 100};

    void SetUp() override {
        system.AddMonster(&a, 10, {0, 0, 0});
        system.AddMonster(&t, 5, {0, 0, 0});
    }
};

TEST_F(AttackAdvantageTest, BlindedTargetGivesAdvantage) {
    t.SetCondition(Condition::Blinded, /*deadline*/999);

    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST_F(AttackAdvantageTest, BlindedAttackerGivesDisadvantage) {
    a.SetCondition(Condition::Blinded, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST_F(AttackAdvantageTest, InvisibleTargetGivesDisadvantage) {
    t.SetCondition(Condition::Invisible, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST_F(AttackAdvantageTest, InvisibleAttackerGivesDisadvantage) {
    a.SetCondition(Condition::Invisible, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST_F(AttackAdvantageTest, ParalyzedTargetAdvantage) {
    t.SetCondition(Condition::Paralyzed, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST_F(AttackAdvantageTest, PetrifiedTargetAdvantage) {
    t.SetCondition(Condition::Petrified, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST_F(AttackAdvantageTest, PoisonedAttackerDisadvantage) {
    a.SetCondition(Condition::Poisoned, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST_F(AttackAdvantageTest, ProneTargetMeleeAdvantageWithin5) {
    system.GetBattlefield().SetPosition(a, {0,0}); system.GetBattlefield().SetPosition(t, {0,0}); // distance 0
    t.SetCondition(Condition::Prone, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST_F(AttackAdvantageTest, ProneTargetRangedDisadvantageBeyond5) {
    system.GetBattlefield().SetPosition(a, {0,0}); system.GetBattlefield().SetPosition(t, {10,0}); // distance 10
    t.SetCondition(Condition::Prone, 999);
    AttackRangedAction ranged = MakeRanged();
    // Base melee logic would mark disadvantage for >5, and ranged keeps it
    EXPECT_EQ(ranged.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST_F(AttackAdvantageTest, RestrainedAttackerDisadvantage) {
    a.SetCondition(Condition::Restrained, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST_F(AttackAdvantageTest, RestrainedTargetAdvantage) {
    t.SetCondition(Condition::Restrained, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST_F(AttackAdvantageTest, StunnedTargetAdvantage) {
    t.SetCondition(Condition::Stunned, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST_F(AttackAdvantageTest, UnconsciousTargetAdvantage) {
    t.SetCondition(Condition::Unconscious, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST_F(AttackAdvantageTest, AdvantageAndDisadvantageCancelToLockedNormal) {
    a.SetCondition(Condition::Blinded, 999);   // disadvantage
    t.SetCondition(Condition::Paralyzed, 999); // advantage
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::NormalUnmutable);
}

/*
// Ranged specific: within 5ft applies disadvantage unless incapacitated (already handled in Action)
TEST_F(AttackAdvantageTest, CloseRangeDisadvantageApplied) {
    a.SetPosition({0,0}); t.SetPosition({3,4}); // distance 5
    AttackRangedAction ranged = MakeRanged();
    EXPECT_EQ(ranged.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST_F(AttackAdvantageTest, CloseRangeDisadvantageStacksAndCancels) {
    a.SetPosition({0,0}); t.SetPosition({0,0}); // <=5
    // Give an advantage state from target condition, then close-range should cancel to locked normal
    t.SetCondition(Condition::Restrained, 999); // advantage to attacker
    AttackRangedAction ranged = MakeRanged();
    EXPECT_EQ(ranged.HasAdvantage(a, t), Advantage::NormalUnmutable);
}
    EXPECT_EQ(ranged.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST(AttackAdvantageRanged, CloseRangeDisadvantageStacksAndCancels) {
    DummyMonster a; DummyMonster t;
    a.SetPosition({0,0}); t.SetPosition({0,0}); // <=5
    // Give an advantage state from target condition, then close-range should cancel to locked normal
    t.SetCondition(Condition::Restrained, 999); // advantage to attacker
    AttackRangedAction ranged = MakeRanged();
    EXPECT_EQ(ranged.HasAdvantage(a, t), Advantage::NormalUnmutable);
}
*/