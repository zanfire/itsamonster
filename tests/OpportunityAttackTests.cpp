#include <gtest/gtest.h>

#include "core/CombatSystem.hpp"
#include "reactions/Reaction.hpp"
#include "monsters/Larvae.hpp"
#include "monsters/AwakenedPlants.hpp"

using namespace itsamonster;

TEST(OpportunityAttack, TriggersAndConsumesReactionOnEnterReach) {
    // Arrange: two monsters and reaction listener on defender
    Larvae attacker;            // will move
    AwakenedPlants defender;    // has OA reaction

    // Place them apart then simulate movement into reach of defender
    attacker.SetPosition({0, 0});
    defender.SetPosition({10, 0});

    OpportunityAttackReaction oa(defender);

    // Register participants in the round so CombatSystem can map trackers
    std::array<Monster*, 2> order{ &attacker, &defender };

    // Act: open a round to initialize trackers, then notify position change to simulate entering reach
    CombatSystem::Instance().Round(order);

    // Simulate attacker moving into reach of defender
    Position oldPos = attacker.GetPosition();
    Position newPos{5, 0}; // within 5ft reach assuming reach=5
    attacker.SetPosition(newPos);
    CombatSystem::Instance().NotifyPositionChanged(attacker, oldPos, newPos);

    // Assert: defender should have used reaction (OA consumed)
    EXPECT_TRUE(CombatSystem::Instance().HasUsedReaction(defender));
}
