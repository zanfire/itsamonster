#include "Match.hpp"
#include "monsters/Monster.hpp"
#include "core/Battlefield.hpp"
#include "core/Dice.hpp"

#include <array>
#include <string_view>
#include <utility>
#include <iostream>
#include <memory>
#include <vector>
#include <functional>

using namespace itsamonster;

std::string Match::GetWinnerName() const {
    std::string winner;
    int win = 0;
    for (const auto& [team, info] : m_teams) {
        if (info.win > win) {
            win = info.win;
            winner = team;
        }
    }
    return winner;
}

double Match::GetAverageRounds(int total_simulations) const {
    return static_cast<double>(m_totalRounds.load()) / total_simulations;
}

void Match::Go(int runs, int seed) {
    auto t1 = std::chrono::steady_clock::now();
    std::atomic<int> threadCounter{ 0 };
    const int numBuckets = std::thread::hardware_concurrency();
    int bucketSize = runs / numBuckets;
    int remainder = runs % numBuckets;

    std::vector<std::thread> threads;
    for (int i = 0; i < numBuckets; ++i) {
        int currentBucketSize = bucketSize + (i < remainder ? 1 : 0);
        threads.emplace_back([&, i, currentBucketSize]() {
            InitThreadDice(seed + i);
            for (int j = 0; j < currentBucketSize; ++j) {
                CombatSystem system(250, 250);
                int init = 0;
                std::vector<std::unique_ptr<Monster>> team1;
                std::vector<std::unique_ptr<Monster>> team2;
                for (const auto& [teamName, teamInfo] : m_teams) {
                    bool isTeam1 = (teamName == GetMonster1Name());
                    for (const auto& member : teamInfo.members) {
                        auto monster = member.factory(system);
                        system.AddMonster(monster.get(), member.initiative, member.pos, isTeam1 ? 1 : 2);
                        if (isTeam1) {
                            team1.push_back(std::move(monster));
                        } else {
                            team2.push_back(std::move(monster));
                        }
                    }
                }
                    
                int rounds = Fight(system, team1, team2);
                m_totalRounds.fetch_add(rounds, std::memory_order_relaxed);
            }
        });
    }
    for (auto& t : threads) t.join();
    auto t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = t2 - t1;
    std::cout << "Elapsed time: " << elapsed.count() << " ms\n";
}

void Match::AddMonster(std::string_view team, MonsterFactoryFunc factory, Position pos, int initiative) {
    m_teams[std::string(team)].members.emplace_back(MemberInfo{ factory, pos, initiative });
    if (m_team1.empty()) m_team1 = team;
    else if (m_team2.empty() && m_team1 != team) m_team2 = team;
}

int Match::Fight(CombatSystem& system, const std::vector<std::unique_ptr<Monster>>& team1,
                            const std::vector<std::unique_ptr<Monster>>& team2) {
    int round = 1;
       
    if (m_darkness) {
        system.GetBattlefield().SetDarkness(true);
        LOG("Darkness is enabled, visibility is reduced.");
    } else {
        system.GetBattlefield().SetDarkness(false);
        LOG("Darkness is disabled, full visibility.");
    }

    auto& tracker = system.GetTurnStatusTracker();
    std::vector<TurnStatus*> status1;
    std::vector<TurnStatus*> status2;
    auto monsters = tracker.GetTurnOrder();
    for (const auto& m : team1) status1.push_back(tracker.GetTurnStatus(m->GetInstanceId()));
    for (const auto& m : team2) status2.push_back(tracker.GetTurnStatus(m->GetInstanceId()));

    auto aliveCount = [](auto& statuses) {
        std::size_t alive = 0;
        for (auto* s : statuses) if (s && !s->dead) ++alive;
        return alive;
    };

    if (Logger::Instance().IsVerbose()) {
        system.GetBattlefield().ShowMap();
    }

    while (aliveCount(status1) > 0 && aliveCount(status2) > 0) {
        if (Logger::Instance().IsVerbose()) {
            std::cout << "-- Round " << round << " --\n";
        }
        system.Round();
        round = system.GetCurrentRound();

        LOG("--- Round ended ---");
    }
    bool team1Won = aliveCount(status1) > 0;
    if (team1Won) {
        m_teams[m_team1].win++;
    } else {
        m_teams[m_team2].win++;
    }
    LOG(std::string("Winner: ") + (team1Won ? m_team1 : m_team2)
        << " team after " << round << " rounds");
    return round;
}
