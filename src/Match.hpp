#pragma once

#include "Logger.hpp"
#include "Monster.hpp"

#include <array>
#include <string_view>
#include <utility>
#include <random>
#include <iostream>
#include <execution>

namespace itsamonster {

template<typename MonsterType1, typename MonsterType2>
struct Match {
    Match() = default;
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
        MonsterType1 m1;
        return m1.GetName();
    }
    std::string_view GetMonster2Name() const {
        MonsterType2 m2;
        return m2.GetName();
    }
    int GetMonster1Wins() const { return m_monsterWin1; }
    int GetMonster2Wins() const { return m_monsterWin2; }

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
                std::mt19937 rng(seed + i);
                int localWin1 = 0, localWin2 = 0;
                for (int j = 0; j < currentBucketSize; ++j) {
                    MonsterType1 m1;
                    MonsterType2 m2;
                    Fight(m1, m2, rng);
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
    void Fight(MonsterType1 &monster1, MonsterType2 &monster2, std::mt19937 &rng) {
        int round = 1;
        LOG("=== New Fight: " << monster1.GetName() << " vs " << monster2.GetName() << " ===");
        while (monster1.GetHP() > 0 && monster2.GetHP() > 0) {
            if (g_verbose) {
                std::cout << "-- Round " << round << " --\n";
                std::cout << "  Status: " << monster1.GetName() << " HP=" << monster1.GetHP() << " | "
                        << monster2.GetName() << " HP=" << monster2.GetHP() << "\n";
                std::cout << "  " << monster1.GetName() << " acts\n";
            }
            monster1.StartTurn(round, rng);
            monster1.TakeAction(monster2, rng);
            monster1.EndTurn(rng);
            if (monster2.GetHP() <= 0) break;
            LOG("  " << monster2.GetName() << " acts\n");
            monster2.StartTurn(round, rng);
            monster2.TakeAction(monster1, rng);
            monster2.EndTurn(rng);

            if (monster1.GetHP() <= 0 || monster2.GetHP() <= 0) break;
            ++round;
            
            LOG("--- Round ended ---");
            LOG("");
        }
        if (monster1.GetHP() > 0) {
            m_monsterWin1++;
        } else {
            m_monsterWin2++;
        }
        if (monster1.GetHP() > 0) {
            LOG("Winner: " << monster1.GetName() << " after " << round << " rounds");
        } else {
            LOG("Winner: " << monster2.GetName() << " after " << round << " rounds");
        }
    }

    std::atomic<int> m_monsterWin1{ 0 };
    std::atomic<int> m_monsterWin2{ 0 };
};

} // namespace itsamonster