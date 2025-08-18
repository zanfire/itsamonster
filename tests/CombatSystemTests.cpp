#include <gtest/gtest.h>

#include "core/CombatSystem.hpp"
#include "reactions/Reaction.hpp"
#include "monsters/Larvae.hpp"
#include "monsters/AwakenedPlants.hpp"
#include <core/Strategy.hpp>
#include "test_utils/MockDice.hpp"

using namespace itsamonster;

TEST(CombatSystem, Test1) {
    Logger::Instance().SetVerbose(true);

    MockDice mock;
    ScopedDiceOverride scoped(&mock);
    EXPECT_CALL(mock, D20(testing::_)).WillRepeatedly(testing::Return(15));
    CombatSystem system;

    // Arrange: two monsters and reaction listener on defender
    AwakenedPlants attacker(system);
    AwakenedPlants defender(system);    // has OA reaction

    system.AddMonster(&attacker, 19, { 0, 0, 0});
    system.AddMonster(&defender, 20, { 60, 60, 0 });

    // Register participants in the round so CombatSystem can map trackers
    std::array<MonsterPtr, 2> order{ &attacker, &defender };

    // Act: open a round to initialize trackers, then notify position change to simulate entering reach
    for (int i = 0; i < 10; ++i) {
        system.Round();
    }
}