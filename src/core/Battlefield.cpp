#include "Battlefield.hpp"

#include "monsters/Monster.hpp"

using namespace itsamonster;

void Battlefield::MoveTowardsInSteps(Monster& mover,
                            const Position& targetPos,
                            double& remainingSpeed,
                            double stopDistance,
                            double stepFeet) {
    if (remainingSpeed <= 0.0) return;
    Position cur = mover.GetPosition();
    while (remainingSpeed > 0.0) {
        double dist = cur.DistanceTo(targetPos);
        if (dist <= stopDistance + 1e-6) break;
        double maxAdvance = std::max(0.0, dist - stopDistance);
        double toMove = std::min({ stepFeet, remainingSpeed, maxAdvance });
        if (toMove <= 0.0) break;

        double ratio = toMove / dist;
        Position next;
        next.x = static_cast<int>(std::round(cur.x + (targetPos.x - cur.x) * ratio));
        next.y = static_cast<int>(std::round(cur.y + (targetPos.y - cur.y) * ratio));
        if (next.x == cur.x && next.y == cur.y) {
            int dx = (targetPos.x > cur.x) ? 1 : (targetPos.x < cur.x) ? -1 : 0;
            int dy = (targetPos.y > cur.y) ? 1 : (targetPos.y < cur.y) ? -1 : 0;
            if (dx == 0 && dy == 0) break;
            next.x = cur.x + dx;
            next.y = cur.y + dy;
        }
        mover.SetPosition(next);
        remainingSpeed -= toMove;
        cur = next;
    }
}

void Battlefield::MoveAwayInSteps(Monster& mover,
                         const Position& fromPos,
                         double& remainingSpeed,
                         double targetDistance,
                         double stepFeet) {
    if (remainingSpeed <= 0.0) return;
    Position cur = mover.GetPosition();
    while (remainingSpeed > 0.0) {
        double dist = cur.DistanceTo(fromPos);
        if (dist >= targetDistance - 1e-6) break;
        double toMove = std::min(stepFeet, remainingSpeed);
        if (toMove <= 0.0) break;

        Position next;
        if (dist <= 1e-9) {
            next = Position{ cur.x + static_cast<int>(std::round(toMove)), cur.y };
        } else {
            double nx = (cur.x - fromPos.x) / dist;
            double ny = (cur.y - fromPos.y) / dist;
            // Advance by at most what's needed to reach targetDistance, but still emit steps
            double maxAdvance = targetDistance - dist;
            double actual = std::min(toMove, std::max(0.0, maxAdvance));
            if (actual <= 0.0) break;
            next.x = static_cast<int>(std::round(cur.x + nx * actual));
            next.y = static_cast<int>(std::round(cur.y + ny * actual));
            if (next.x == cur.x && next.y == cur.y) {
                int dx = (nx > 0) ? 1 : (nx < 0) ? -1 : 0;
                int dy = (ny > 0) ? 1 : (ny < 0) ? -1 : 0;
                if (dx == 0 && dy == 0) { next.x = cur.x + 1; next.y = cur.y; }
                else { next.x = cur.x + dx; next.y = cur.y + dy; }
            }
        }
        mover.SetPosition(next);
        remainingSpeed -= toMove;
        cur = next;
    }
}
