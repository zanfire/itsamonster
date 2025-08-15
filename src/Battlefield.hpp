#pragma once

#include <cmath>

namespace itsamonster {

struct Position {
    int x{0};
    int y{0};

    double DistanceTo(const Position &other) const {
        int dx = x - other.x;
        int dy = y - other.y;
        return std::sqrt(static_cast<double>(dx*dx + dy*dy));
    }
};

class Monster; // forward declaration

class Battlefield {
public:
    Battlefield(int w, int h) : m_width(w), m_height(h) {}

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    Position Clamp(Position p) const {
        if (p.x < 0) p.x = 0; if (p.x >= m_width) p.x = m_width - 1;
        if (p.y < 0) p.y = 0; if (p.y >= m_height) p.y = m_height - 1;
        return p;
    }
private:
    int m_width;
    int m_height;
};

} // namespace itsamonster
