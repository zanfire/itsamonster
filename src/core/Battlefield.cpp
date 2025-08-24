#include "Battlefield.hpp"

#include "monsters/Monster.hpp"
#include "core/CombatSystem.hpp"

using namespace itsamonster;

namespace {
    // 5e space occupied per size (in feet). Tiny: 2.5ft is often used, but on grid it's 2.5ft; we approximate to 2.5.
    inline double SizeSpaceFeet(MonsterSize s) {
        switch (s) {
            case MonsterSize::Tiny:        return 2.5;  // occupies a 2.5 ft square
            case MonsterSize::Small:       return 5.0;  // 1x1 square
            case MonsterSize::Medium:      return 5.0;  // 1x1 square
            case MonsterSize::Large:       return 10.0; // 2x2 squares
            case MonsterSize::Huge:        return 15.0; // 3x3 squares
            case MonsterSize::Gargantuan:  return 20.0; // 4x4+ squares (simplified)
            default:                       return 5.0;
        }
    }

    // Radius from creature center to edge (feet), assuming square approximated as circle for distances
    inline double SizeRadiusFeet(MonsterSize s) {
        return SizeSpaceFeet(s) / 2.0;
    }
}

bool Battlefield::OnTurnEvent(TurnEvent ev, EventPayload* payload) {
    auto monsterPayload = dynamic_cast<MonsterPayload*>(payload);

    if (ev == TurnEvent::MonsterEnter && dynamic_cast<MonsterEnterPayload*>(payload) != nullptr) {
        auto monsterEnterPayload = dynamic_cast<MonsterEnterPayload*>(payload);
        SetPosition(*monsterEnterPayload->monster, monsterEnterPayload->spawnPos);
    }
    return true;
}

double Battlefield::MoveTowardsInSteps(Monster& mover,
                            const Position& targetPos,
                            double& remainingSpeed,
                            double stopDistance,
                            double stepFeet) {
    if (remainingSpeed <= 0.0) return 0.0f;
    Position cur = *GetPosition(mover.GetInstanceId());
    // Adjust stop threshold by mover's size radius since targetPos is a point (likely a target's center)
    double moverRadius = SizeRadiusFeet(mover.GetMonsterSize());
    double initialRemainingSpeed = remainingSpeed;
    while (remainingSpeed > 0.0) {
        double dist = cur.DistanceTo(targetPos);
        // We assume targetPos is the other creature's center if used that way; preserve original behavior
        // but remove the hardcoded 2.5ft and instead use mover's radius so we don't overlap into target space.
        if (dist <= stopDistance + moverRadius + 1e-6) break;
        double maxAdvance = std::max(0.0, dist - (stopDistance + moverRadius));
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
        SetPosition(mover, next);
        remainingSpeed -= toMove;
        cur = next;
    }
    return initialRemainingSpeed - remainingSpeed;
}

double Battlefield::MoveTowardsInSteps(Monster& mover,
                            const Monster& target,
                            double& remainingSpeed,
                            double stopDistance,
                            double stepFeet) {
    if (remainingSpeed <= 0.0) return 0.0f;
    auto targetPosOpt = GetPosition(target.GetInstanceId());
    if (!targetPosOpt) return 0.0f;
    Position targetPos = *targetPosOpt;
    Position cur = *GetPosition(mover.GetInstanceId());
    double initialRemainingSpeed = remainingSpeed;

    const double moverRadius = SizeRadiusFeet(mover.GetMonsterSize());
    const double targetRadius = SizeRadiusFeet(target.GetMonsterSize());
    const double desiredEdgeDistance = stopDistance; // desired edge-to-edge spacing

    while (remainingSpeed > 0.0) {
        double centerDist = cur.DistanceTo(targetPos);
        double edgeDist = centerDist - (moverRadius + targetRadius);
        if (edgeDist <= desiredEdgeDistance + 1e-6) break;

        // We can advance until edgeDist equals desiredEdgeDistance
        double maxAdvance = std::max(0.0, centerDist - (desiredEdgeDistance + moverRadius + targetRadius));
        double toMove = std::min({ stepFeet, remainingSpeed, maxAdvance });
        if (toMove <= 0.0) break;

        double ratio = toMove / centerDist;
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
        SetPosition(mover, next);
        remainingSpeed -= toMove;
        cur = next;
        // Update target position in case it can move reactively due to events
        auto np = GetPosition(target.GetInstanceId());
        if (np) targetPos = *np;
    }

    if (Logger::Instance().IsVerbose()) {
        ShowMap();
    }
    return initialRemainingSpeed - remainingSpeed;
}

void Battlefield::MoveAwayInSteps(Monster& mover,
                         const Position& fromPos,
                         double& remainingSpeed,
                         double targetDistance,
                         double stepFeet) {
    if (remainingSpeed <= 0.0) return;
    Position cur = *GetPosition(mover.GetInstanceId());
    while (remainingSpeed > 0.0) {
        double dist = cur.DistanceTo(fromPos);
        // Ensure we keep at least targetDistance from the other edge; account for mover radius when fromPos is a center
        double moverRadius = SizeRadiusFeet(mover.GetMonsterSize());
        if (dist - moverRadius >= targetDistance - 1e-6) break;
        double toMove = std::min(stepFeet, remainingSpeed);
        if (toMove <= 0.0) break;

        Position next;
        if (dist <= 1e-9) {
            next = Position{ cur.x + static_cast<int>(std::round(toMove)), cur.y };
        } else {
            double nx = (cur.x - fromPos.x) / dist;
            double ny = (cur.y - fromPos.y) / dist;
            // Advance by at most what's needed to reach targetDistance, but still emit steps
            double maxAdvance = (targetDistance + moverRadius) - dist;
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
        SetPosition(mover, next);
        remainingSpeed -= toMove;
        cur = next;
    }

    if (Logger::Instance().IsVerbose()) {
        ShowMap();
    }
}

double Battlefield::GetDistance(MonsterInstanceId a, MonsterInstanceId b) const {
    auto posA = GetPosition(a);
    auto posB = GetPosition(b);
    if (posA && posB) {
        // Try to resolve monsters to get their sizes
        // We rely on TurnStatusTracker to map ids -> Monster* stored in order list
        const Monster* ma = nullptr;
        const Monster* mb = nullptr;
        for (auto* m : m_system.GetTurnStatusTracker().GetTurnOrder()) {
            if (m && m->GetInstanceId() == a) ma = m;
            if (m && m->GetInstanceId() == b) mb = m;
        }
        double center = posA->DistanceTo(*posB);
        if (ma && mb) {
            double ra = SizeRadiusFeet(ma->GetMonsterSize());
            double rb = SizeRadiusFeet(mb->GetMonsterSize());
            return std::max(0.0, center - (ra + rb)); // edge-to-edge distance; never negative
        }
        return center; // fallback
    }
    return -1.0; // Invalid distance if either position is not set
}

void Battlefield::SetPosition(Monster& monster, Position p) {
    p = Clamp(p);
    std::optional<Position> oldPos{};
    auto it = m_positions.find(monster.GetInstanceId());
    if (it != m_positions.end()) {
        oldPos = it->second;
    }

    // Allow listeners to alter the intended new position (e.g., forced movement prevention, teleport tweaks)
    PositionPayload payload;
    payload.monster = &monster;
    payload.oldPos = oldPos;
    payload.newPos = p;
    m_system.NotifyTurnEvent(TurnEvent::BeforeMove, &payload);
    p = Clamp(payload.newPos);

    m_positions[monster.GetInstanceId()] = p;
    m_system.NotifyPositionChanged(monster, oldPos, p);

    payload.oldPos = oldPos;
    payload.newPos = p;
    m_system.NotifyTurnEvent(TurnEvent::AfterMove, &payload);
}

std::optional<Position> Battlefield::GetPosition(MonsterInstanceId monster) const {
    auto it = m_positions.find(monster);
    if (it != m_positions.end()) {
        return it->second;
    }
    return std::nullopt; // Default position if not found
}

void Battlefield::SetDarkness(bool darkness) {
    if (darkness) {
        auto monsters = m_system.GetTurnStatusTracker().GetTurnOrder();
        for (auto& monster : monsters) {
            if (monster && !monster->HasDarkvision()) {
                monster->SetCondition(Condition::Blinded, std::numeric_limits<int>::max());
            }
        }
    }
}

void Battlefield::ShowMap() const {
    std::string head;
    std::string line;
    std::string tail;

    int width = int(m_width / 5.0f);
    int height = int(m_height / 5.0f);
    for (int i = 0; i < width; i++) {
        if (i == 0) head = "X";
        else if (i == (width - 1)) head += "X";
        else head += "-";

        if (i == 0) line = "|";
        else if (i == (width - 1)) line += "|";
        else line += " ";
    }
    tail = head;

    std::cout << head << std::endl;
    for (int i = 0; i < height; i++) {
        std::string curLine = line;
        for (const auto& [id, pos] : m_positions) {
            if ((pos.y / 5) == i) {
                auto status = m_system.GetTurnStatusTracker().GetTurnStatus(id);
                if (curLine[pos.x / 5] != ' ') curLine[pos.x / 5 + 1] = (status->dead) ? 'X' : std::to_string(id)[0];
                else curLine[pos.x / 5] = (status->dead) ? 'X' : std::to_string(id)[0];
            }
        }
        std::cout << curLine << std::endl;
    }
    std::cout << tail << std::endl;
}