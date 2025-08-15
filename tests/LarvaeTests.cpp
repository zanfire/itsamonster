#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "monsters/Larvae.hpp"
#include "monsters/YoungGoldDragon.hpp"
#include <random>

using namespace itsamonster;

TEST(LarvaeTest, PiercingResistanceTrue) {
    Larvae larva;
    EXPECT_TRUE(larva.IsResistant(DamageType::Piercing));
}

TEST(LarvaeTest, DarkvisionTrue) {
    Larvae larva;
    EXPECT_TRUE(larva.HasDarkvision());
}

// Placeholder for future gMock usage when interfaces are introduced.
