#pragma once

#include <random>
#include <algorithm>
#include "Types.hpp"
#include <memory>

namespace itsamonster {

struct Dice {
    virtual ~Dice() = default;
    virtual int Roll(int sides) = 0;
    virtual int D20(Advantage advantage) = 0;
};

class RandomDice : public Dice {
public:
    explicit RandomDice(uint32_t seed) : m_rng(seed) {}
    int Roll(int sides) override {
        std::uniform_int_distribution<int> dist(1, sides);
        return dist(m_rng);
    }
    int D20(Advantage advantage) override {
        switch (advantage) {
            case Advantage::Advantage:
                return std::max(Roll(20), Roll(20));
            case Advantage::Disadvantage:
                return std::min(Roll(20), Roll(20));
            case Advantage::Normal:
            case Advantage::NormalUnmutable:
            default:
                return Roll(20);
        }
    }
private:
    std::mt19937 m_rng;
};

} // namespace itsamonster

namespace itsamonster {
// Thread-local dice management
extern thread_local Dice* g_threadDice; // non-owning pointer

// Retrieve current thread's dice (must have been initialized)
inline Dice& GetDice() { return *g_threadDice; }

// Set the current thread dice pointer (non-owning)
void SetDice(Dice* dice);

// Initialize a thread-local RandomDice with provided seed and set as active.
void InitThreadDice(uint32_t seed);

// RAII helper for temporarily overriding the thread dice (e.g. in tests)
struct ScopedDiceOverride {
    Dice* m_prev;
    explicit ScopedDiceOverride(Dice* replacement) : m_prev(g_threadDice) { g_threadDice = replacement; }
    ~ScopedDiceOverride() { g_threadDice = m_prev; }
};
}
