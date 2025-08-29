#pragma once

#include "monsters/Monster.hpp"

#include <string_view>
#include <utility>
#include <memory>
#include <vector>
#include <functional>

namespace itsamonster {

using MonsterFactoryFunc = std::function<std::unique_ptr<Monster>(CombatSystem&)>;

struct Match {
    struct MemberInfo {
        MonsterFactoryFunc factory;
        Position pos;
        int initiative;
    };

    struct TeamInfo {
        std::vector<MemberInfo> members;
        int win{ 0 };
    };

    Match(bool in_darkness) : m_darkness(in_darkness) {}
    virtual ~Match() = default;

    std::string GetWinnerName() const;

    std::string GetMonster1Name() const {
        return m_team1;
    }

    std::string GetMonster2Name() const {
        return m_team2;
    }

    int GetMonster1Wins() const { return m_teams.at(m_team1).win; }
    int GetMonster2Wins() const { return m_teams.at(m_team2).win; }

    double GetAverageRounds(int total_simulations) const;
    void Go(int runs, int seed);
    void AddMonster(std::string_view team, MonsterFactoryFunc factory, Position pos, int initiative);

private:
    int Fight(CombatSystem& system, const std::vector<std::unique_ptr<Monster>>& team1,
        const std::vector<std::unique_ptr<Monster>>& team2);

    std::string m_team1;
    std::string m_team2; 
    std::atomic<int> m_totalRounds{ 0 };
    bool m_darkness{ false };
    std::map<std::string, TeamInfo> m_teams;
};

} // namespace itsamonster