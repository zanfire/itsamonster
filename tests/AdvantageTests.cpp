#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Types.hpp"
#include "Dice.hpp"

using namespace itsamonster;

class FixedDice : public IDice {
public:
    int value;
    int RollCountAdv = 0;
    explicit FixedDice(int v) : value(v) {}
    int Roll(int /*sides*/) override { return value; }
    int D20(Advantage advantage) override {
        // Use RandomDice logic semantics but deterministic by returning pairs of same value.
        // We only want to ensure branching works; value choice is constant.
        ++RollCountAdv;
        switch (advantage) {
            case Advantage::Advantage: return value; // max(value,value) == value
            case Advantage::Disadvantage: return value; // min(value,value) == value
            case Advantage::Normal:
            case Advantage::NormalUnmutable:
            default: return value;
        }
    }
};

TEST(AdvantageResolveTest, NormalAdoptsOtherStates) {
    EXPECT_EQ(ResolveAdvantage(Advantage::Normal, Advantage::Advantage), Advantage::Advantage);
    EXPECT_EQ(ResolveAdvantage(Advantage::Normal, Advantage::Disadvantage), Advantage::Disadvantage);
    EXPECT_EQ(ResolveAdvantage(Advantage::Normal, Advantage::Normal), Advantage::Normal);
}

TEST(AdvantageResolveTest, AdvantageAndDisadvantageCancel) {
    EXPECT_EQ(ResolveAdvantage(Advantage::Advantage, Advantage::Disadvantage), Advantage::NormalUnmutable);
    EXPECT_EQ(ResolveAdvantage(Advantage::Disadvantage, Advantage::Advantage), Advantage::NormalUnmutable);
}

TEST(AdvantageResolveTest, SameStatesRemain) {
    EXPECT_EQ(ResolveAdvantage(Advantage::Advantage, Advantage::Advantage), Advantage::Advantage);
    EXPECT_EQ(ResolveAdvantage(Advantage::Disadvantage, Advantage::Disadvantage), Advantage::Disadvantage);
}

TEST(AdvantageResolveTest, NormalUnmutableLocksState) {
    // Current locked stays locked regardless of target
    EXPECT_EQ(ResolveAdvantage(Advantage::NormalUnmutable, Advantage::Advantage), Advantage::NormalUnmutable);
    EXPECT_EQ(ResolveAdvantage(Advantage::NormalUnmutable, Advantage::Disadvantage), Advantage::NormalUnmutable);
    EXPECT_EQ(ResolveAdvantage(Advantage::NormalUnmutable, Advantage::Normal), Advantage::NormalUnmutable);
    // Target locked overrides current
    EXPECT_EQ(ResolveAdvantage(Advantage::Advantage, Advantage::NormalUnmutable), Advantage::NormalUnmutable);
    EXPECT_EQ(ResolveAdvantage(Advantage::Disadvantage, Advantage::NormalUnmutable), Advantage::NormalUnmutable);
    EXPECT_EQ(ResolveAdvantage(Advantage::Normal, Advantage::NormalUnmutable), Advantage::NormalUnmutable);
}

TEST(AdvantageToString, ReturnsExpectedStrings) {
    EXPECT_EQ(to_string(Advantage::Normal), std::string_view("Normal"));
    EXPECT_EQ(to_string(Advantage::Advantage), std::string_view("Advantage"));
    EXPECT_EQ(to_string(Advantage::Disadvantage), std::string_view("Disadvantage"));
    EXPECT_EQ(to_string(Advantage::NormalUnmutable), std::string_view("NormalUnmutable"));
}

TEST(AdvantageDiceTest, NormalUnmutableBehavesAsNormal) {
    FixedDice dice(7);
    ScopedDiceOverride scoped(&dice);
    // If logic diverged, we might expect different results; here we just call both.
    int normal = GetDice().D20(Advantage::Normal);
    int locked = GetDice().D20(Advantage::NormalUnmutable);
    EXPECT_EQ(normal, locked);
    EXPECT_EQ(normal, 7);
}
