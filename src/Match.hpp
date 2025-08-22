#pragma once

#include "Logger.hpp"
#include "monsters/Monster.hpp"
#include "core/Battlefield.hpp"
#include "core/Dice.hpp"

#include <array>
#include <string_view>
#include <utility>
#include <random>
#include <iostream>
#include <execution>
#include <memory>
#include <vector>
#include <type_traits>

namespace itsamonster {

// Helper to detect std::array<T, N>
template <typename T>
struct is_std_array : std::false_type {};
template <typename T, std::size_t N>
struct is_std_array<std::array<T, N>> : std::true_type {};

template<typename Team1Array, typename Team2Array>
struct Match {
    static_assert(is_std_array<Team1Array>::value, "Team1 must be a std::array of a Monster type");
    static_assert(is_std_array<Team2Array>::value, "Team2 must be a std::array of a Monster type");

    using MonsterType1 = typename Team1Array::value_type;
    using MonsterType2 = typename Team2Array::value_type;
    static constexpr std::size_t Team1Size = std::tuple_size<Team1Array>::value;
    static constexpr std::size_t Team2Size = std::tuple_size<Team2Array>::value;

    Match(bool in_darkness) : m_darkness(in_darkness) {}
    virtual ~Match() = default;

    std::string_view GetWinnerName() const {
        if (m_monsterWin1 > m_monsterWin2) {
            MonsterType1 m1;
            return m1.GetName();
        } else if (m_monsterWin2 > m_monsterWin1) {
            MonsterType2 m2;
            return m2.GetName();
        } else {
            return "It's a tie!";
        }
    }

    std::string_view GetMonster1Name() const {
        CombatSystem system;
        MonsterType1 m1(system);
        return m1.GetName();
    }
    std::string_view GetMonster2Name() const {
        CombatSystem system;
        MonsterType2 m2(system);
        return m2.GetName();
    }
    int GetMonster1Wins() const { return m_monsterWin1; }
    int GetMonster2Wins() const { return m_monsterWin2; }

    double GetAverageRounds(int total_simulations) const {
        return static_cast<double>(m_totalRounds.load()) / total_simulations;
    }

    void Go(int runs, int seed) {
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
                    // Instantiate team 1 monsters
                    std::array<std::unique_ptr<MonsterType1>, Team1Size> team1{};
                    for (std::size_t k = 0; k < Team1Size; ++k) {
                        team1[k] = std::make_unique<MonsterType1>(system);
                        // Spread along Y axis; place on the right side
                        int y = 150 + static_cast<int>(k) * 10;
                        system.AddMonster(team1[k].get(), 20 - static_cast<int>(k), { 40, y, 0 }, 1);
                    }
                    // Instantiate team 2 monsters
                    std::array<std::unique_ptr<MonsterType2>, Team2Size> team2{};
                    for (std::size_t k = 0; k < Team2Size; ++k) {
                        team2[k] = std::make_unique<MonsterType2>(system);
                        // Spread along Y axis; place on the left side
                        int y = 100 + static_cast<int>(k) * 10;
                        system.AddMonster(team2[k].get(), 10 - static_cast<int>(k), { 30, y, 0 }, 2);
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

private:
    int Fight(
        CombatSystem& system,
        std::array<std::unique_ptr<MonsterType1>, Team1Size>& team1,
        std::array<std::unique_ptr<MonsterType2>, Team2Size>& team2) {
        int round = 1;
        LOG("=== New Team Fight: " << MonsterType1( system ).GetName() << " x" << Team1Size
                                   << " vs " << MonsterType2( system ).GetName() << " x" << Team2Size << " ===");

        if (m_darkness) {
            system.GetBattlefield().SetDarkness(true);
            LOG("Darkness is enabled, visibility is reduced.");
        } else {
            system.GetBattlefield().SetDarkness(false);
            LOG("Darkness is disabled, full visibility.");
        }

        auto& tracker = system.GetTurnStatusTracker();
        std::array<TurnStatus*, Team1Size> status1{};
        std::array<TurnStatus*, Team2Size> status2{};
        for (std::size_t i = 0; i < Team1Size; ++i) status1[i] = tracker.GetTurnStatus(team1[i]->GetInstanceId());
        for (std::size_t i = 0; i < Team2Size; ++i) status2[i] = tracker.GetTurnStatus(team2[i]->GetInstanceId());

        auto aliveCount = [](auto& statuses) {
            std::size_t alive = 0;
            for (auto* s : statuses) if (s && !s->dead) ++alive;
            return alive;
        };

        while (aliveCount(status1) > 0 && aliveCount(status2) > 0) {
            if (Logger::Instance().IsVerbose()) {
                std::cout << "-- Round " << round << " --\n";
            }
            system.Round();
            round = system.GetCurrentRound();

            LOG("--- Round ended ---");
            LOG("");
        }
        bool team1Won = aliveCount(status1) > 0;
        if (team1Won) {
            m_monsterWin1++;
        } else {
            m_monsterWin2++;
        }
        LOG(std::string("Winner: ") + (team1Won ? std::string(MonsterType1(system).GetName()) : std::string(MonsterType2(system).GetName()))
            << " team after " << round << " rounds");
        return round;
    }

    std::atomic<int> m_monsterWin1{ 0 };
    std::atomic<int> m_monsterWin2{ 0 };
    std::atomic<int> m_totalRounds{ 0 };
    bool m_darkness{ false };
};

} // namespace itsamonster