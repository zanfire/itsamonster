#pragma once

#include "monsters/Monster.hpp"
#include "core/TurnEvent.hpp"

namespace itsamonster {

struct Action : public TurnEventListener {
    Action() = default;
    virtual ~Action() = default;

    void Perform(Monster& attacker, Monster& target);

    virtual bool IsInRange(const Monster& attacker, const Monster& target) const = 0;

protected:
    virtual void Execute(Monster& attacker, Monster& target) = 0;
};

} // namespace itsamonster