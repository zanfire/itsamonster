#pragma once

#include "Logger.hpp"
#include "monsters/Monster.hpp"

namespace itsamonster {

struct Action {
    Action() = default;
    virtual ~Action() = default;

    void Perform(Monster& attacker, Monster& target);

    virtual bool IsInRange(const Monster& attacker, const Monster& target) const = 0;

    bool CanAttack(const Monster& attacker, const Monster& target) const;

protected:
    virtual void Execute(Monster& attacker, Monster& target) = 0;
};

} // namespace itsamonster