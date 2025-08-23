#include <gtest/gtest.h>

#include "actions/DashAction.hpp"
#include "core/CombatSystem.hpp"
#include "monsters/Monster.hpp"

using namespace itsamonster;

struct DashTestMonster : public Monster {
    DashTestMonster(CombatSystem& system, std::string_view name, int speed)
        : Monster(system, name, 30, 10, speed, {
            std::make_pair(10,0), std::make_pair(10,0), std::make_pair(10,0),
            std::make_pair(10,0), std::make_pair(10,0), std::make_pair(10,0) }) {}
};

TEST(DashActionTest, DashMovesTowardsTargetUpToSpeed) {
    CombatSystem system(200, 200);
    DashTestMonster a(system, "Dasher", 30);
    DashTestMonster b(system, "Target", 30);

    // Add monsters to battlefield with starting positions and initiatives/factions
    system.AddMonster(&a, /*init*/20, /*pos*/{0,0,0}, /*faction*/1);
    system.AddMonster(&b, /*init*/10, /*pos*/{60,0,0}, /*faction*/2);

    auto d0 = system.GetBattlefield().GetDistance(a.GetInstanceId(), b.GetInstanceId());
    ASSERT_GT(d0, 0.0);

    DashAction dash(system, "Dash", /*stopDistance*/5.0);
    dash.Perform(a, b);

    auto d1 = system.GetBattlefield().GetDistance(a.GetInstanceId(), b.GetInstanceId());
    // Should reduce distance by up to speed but not closer than stopDistance
    EXPECT_LT(d1, d0);
    EXPECT_GE(d1, 5.0);
}
