#ifndef SPHERICAL_HEX_HEX_COORD_H
#define SPHERICAL_HEX_HEX_COORD_H

#include <cstdint>
#include <functional>
#include <vector>

struct HexCoord {
    int32_t q;
    int32_t r;
    int32_t s; // derived

    HexCoord(int32_t q_=0, int32_t r_=0) : q(q_), r(r_), s(-q_ - r_) {}

    bool operator==(const HexCoord &other) const {
        return q == other.q && r == other.r; // s implied
    }

    std::vector<HexCoord> neighbors() const {
        static const int dirs[6][2] = {
            {1,0},{1,-1},{0,-1},{-1,0},{-1,1},{0,1}
        };
        std::vector<HexCoord> out;
        out.reserve(6);
        for (auto &d : dirs) {
            out.emplace_back(q + d[0], r + d[1]);
        }
        return out;
    }
};

struct HexCoordHash {
    std::size_t operator()(const HexCoord &hc) const noexcept {
        // simple hash combine
        std::size_t h1 = std::hash<int32_t>{}(hc.q);
        std::size_t h2 = std::hash<int32_t>{}(hc.r);
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1<<6) + (h1>>2));
    }
};

#endif // SPHERICAL_HEX_HEX_COORD_H
