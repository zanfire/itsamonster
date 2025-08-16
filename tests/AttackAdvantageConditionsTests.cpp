#include <gtest/gtest.h>
#include <gmock/gmock.h>

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

TEST(AttackAdvantage, BlindedTargetGivesAdvantage) {
    DummyMonster a("A");
    DummyMonster t("T");
    t.SetPosition({0,0}); a.SetPosition({0,0});
    t.SetCondition(Condition::Blinded, /*deadline*/999);

    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST(AttackAdvantage, BlindedAttackerGivesDisadvantage) {
    DummyMonster a("A");
    DummyMonster t("T");
    a.SetCondition(Condition::Blinded, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST(AttackAdvantage, InvisibleTargetGivesDisadvantage) {
    DummyMonster a("A");
    DummyMonster t("T");
    t.SetCondition(Condition::Invisible, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST(AttackAdvantage, InvisibleAttackerGivesDisadvantage) {
    DummyMonster a("A");
    DummyMonster t("T");
    a.SetCondition(Condition::Invisible, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST(AttackAdvantage, ParalyzedTargetAdvantage) {
    DummyMonster a("A");
    DummyMonster t("T");
    t.SetCondition(Condition::Paralyzed, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST(AttackAdvantage, PetrifiedTargetAdvantage) {
    DummyMonster a("A"); DummyMonster t("T");
    t.SetCondition(Condition::Petrified, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST(AttackAdvantage, PoisonedAttackerDisadvantage) {
    DummyMonster a("A"); DummyMonster t("T");
    a.SetCondition(Condition::Poisoned, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST(AttackAdvantage, ProneTargetMeleeAdvantageWithin5) {
    DummyMonster a; DummyMonster t;
    a.SetPosition({0,0}); t.SetPosition({0,0}); // distance 0
    t.SetCondition(Condition::Prone, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST(AttackAdvantage, ProneTargetRangedDisadvantageBeyond5) {
    DummyMonster a; DummyMonster t;
    a.SetPosition({0,0}); t.SetPosition({10,0}); // distance 10
    t.SetCondition(Condition::Prone, 999);
    AttackRangedAction ranged = MakeRanged();
    // Base melee logic would mark disadvantage for >5, and ranged keeps it
    EXPECT_EQ(ranged.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST(AttackAdvantage, RestrainedAttackerDisadvantage) {
    DummyMonster a; DummyMonster t;
    a.SetCondition(Condition::Restrained, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Disadvantage);
}

TEST(AttackAdvantage, RestrainedTargetAdvantage) {
    DummyMonster a; DummyMonster t;
    t.SetCondition(Condition::Restrained, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST(AttackAdvantage, StunnedTargetAdvantage) {
    DummyMonster a; DummyMonster t;
    t.SetCondition(Condition::Stunned, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST(AttackAdvantage, UnconsciousTargetAdvantage) {
    DummyMonster a; DummyMonster t;
    t.SetCondition(Condition::Unconscious, 999);
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::Advantage);
}

TEST(AttackAdvantage, AdvantageAndDisadvantageCancelToLockedNormal) {
    DummyMonster a; DummyMonster t;
    a.SetCondition(Condition::Blinded, 999);   // disadvantage
    t.SetCondition(Condition::Paralyzed, 999); // advantage
    AttackMeleeAction melee = MakeMelee();
    EXPECT_EQ(melee.HasAdvantage(a, t), Advantage::NormalUnmutable);
}

// Ranged specific: within 5ft applies disadvantage unless incapacitated (already handled in Action)
TEST(AttackAdvantageRanged, CloseRangeDisadvantageApplied) {
    DummyMonster a; DummyMonster t;
    a.SetPosition({0,0}); t.SetPosition({3,4}); // distance 5
    AttackRangedAction ranged = MakeRanged();
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
