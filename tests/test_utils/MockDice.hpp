#pragma once

#include <gmock/gmock.h>
#include "Dice.hpp"

namespace itsamonster {

class MockDice : public IDice {
public:
    MOCK_METHOD(int, Roll, (int sides), (override));
    MOCK_METHOD(int, D20, (Advantage advantage), (override));
};

} // namespace itsamonster
