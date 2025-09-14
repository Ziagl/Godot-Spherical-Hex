#include "hex_grid.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>

using namespace godot;

static inline Vector2 hex_to_pixel(const HexCoord &h, float size) {
    const float SQRT_3 = 1.7320508075688772f;
    return {
        size * (1.5f * (float)h.q),
        size * (SQRT_3/2.f * (float)h.q + SQRT_3 * (float)h.r)
    };
}

static inline HexCoord pixel_to_hex(const Vector2 &p, float size) {
    const float SQRT_3 = 1.7320508075688772f;
    float q = (2.f/3.f * p.x) / size;
    float r = (-1.f/3.f * p.x + SQRT_3/3.f * p.y) / size;
    float s = -q - r;
    float rq = std::round(q);
    float rr = std::round(r);
    float rs = std::round(s);
    float q_diff = std::fabs(rq - q);
    float r_diff = std::fabs(rr - r);
    float s_diff = std::fabs(rs - s);
    if (q_diff > r_diff && q_diff > s_diff) {
        rq = -rr - rs;
    } else if (r_diff > s_diff) {
        rr = -rq - rs;
    } else {
        rs = -rq - rr;
    }
    return HexCoord((int)rq, (int)rr);
}

static inline Vector3 project_onto_sphere(const Vector3 &p) { return p.normalized(); }

void HexGrid::generate_on_face(const IcosahedronFace &face) {
    Vector3 center = face.center().normalized();
    Vector3 normal = face.normal;
    Vector3 tangent = (face.v[1] - face.v[0]).normalized();
    Vector3 bitangent = normal.cross(tangent);
    Basis orientation(tangent, bitangent, normal);

    std::vector<HexCoord> local;
    for (int q = -settings.grid_radius; q <= settings.grid_radius; ++q) {
        for (int r = -settings.grid_radius; r <= settings.grid_radius; ++r) {
            int s = -q - r;
            if (std::abs(s) <= settings.grid_radius) {
                HexCoord hc(q, r);
                Vector2 pos2d = hex_to_pixel(hc, settings.hex_size);
                Vector3 pos3d = orientation.xform(Vector3(pos2d.x, pos2d.y, 0));
                Vector3 world_pos = project_onto_sphere(center + pos3d);
                hex_positions.emplace(hc, world_pos);
                local.push_back(hc);
            }
        }
    }

    FaceGridData data{face, local, center, orientation};
    face_grids.push_back(std::move(data));
}

std::vector<Vector3> HexGrid::get_neighbor_positions(const HexCoord &coord) const {
    std::vector<Vector3> out;
    for (auto &n : coord.neighbors()) {
        auto it = hex_positions.find(n);
        if (it != hex_positions.end()) out.push_back(it->second);
    }
    return out;
}
