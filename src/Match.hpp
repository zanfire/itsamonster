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

namespace itsamonster {

template<typename MonsterType1, typename MonsterType2>
struct Match {
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
                    CombatSystem system;
                    MonsterType1 m1(system);
                    MonsterType2 m2(system);
                    system.AddMonster(&m1, 20, { 10, 10, 0 });
                    system.AddMonster(&m2, 10, { 50, 50, 0 });
                    int rounds = Fight(system, m1, m2);
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
    template<typename MonsterType1, typename MonsterType2>
    int Fight(CombatSystem& system, MonsterType1 &monster1, MonsterType2 &monster2) {
        int round = 1;
        LOG("=== New Fight: " << monster1.GetName() << " vs " << monster2.GetName() << " ===");
        //if (m_darkness) {
        //    LOG("The fight is happening in darkness!");
        //    if (!monster1.HasDarkvision()) {
        //        LOG("  " << monster1.GetName() << " is blinded by the darkness.");
        //        monster1.SetCondition(Condition::Blinded,  std::numeric_limits<int>::max());
        //    }
        //    if (!monster2.HasDarkvision()) {
        //        LOG("  " << monster2.GetName() << " is blinded by the darkness.");
        //        monster2.SetCondition(Condition::Blinded,  std::numeric_limits<int>::max());
        //    }
        //}
        auto& tracker = system.GetTurnStatusTracker();
        auto* status1 = tracker.GetTurnStatus(monster1.GetInstanceId());
        auto* status2 = tracker.GetTurnStatus(monster2.GetInstanceId());
        while (!status1->dead && !status2->dead) {
            if (Logger::Instance().IsVerbose()) {
                std::cout << "-- Round " << round << " --\n";
                std::cout << "  Status: " << monster1.GetName() << " HP=" << (monster1.GetHP() - status1->damageTaken) << " | "
                          << monster2.GetName() << " HP=" << (monster2.GetHP() - status2->damageTaken) << "\n";
                std::cout << "  " << monster1.GetName() << " acts\n";
            }
            
            system.Round();

            LOG("--- Round ended ---");
            LOG("");
        }
        if (status1->dead) {
            m_monsterWin1++;
        } else {
            m_monsterWin2++;
        }
        if (status1->dead) {
            LOG("Winner: " << monster1.GetName() << " after " << round << " rounds");
        } else {
            LOG("Winner: " << monster2.GetName() << " after " << round << " rounds");
        }
        return round;
    }

    std::atomic<int> m_monsterWin1{ 0 };
    std::atomic<int> m_monsterWin2{ 0 };
    std::atomic<int> m_totalRounds{ 0 };
    bool m_darkness{ false };
};

} // namespace itsamonster