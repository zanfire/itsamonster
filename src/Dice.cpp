#include "Dice.hpp"

namespace itsamonster {
thread_local IDice* g_threadDice = nullptr;

void SetDice(IDice* dice) { g_threadDice = dice; }

void InitThreadDice(uint32_t seed) {
    // Own the dice instance per thread.
    thread_local std::unique_ptr<RandomDice> owned;
    owned = std::make_unique<RandomDice>(seed);
    g_threadDice = owned.get();
}
} // namespace itsamonster
