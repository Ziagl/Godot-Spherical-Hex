#ifndef SPHERICAL_HEX_HEX_GRID_H
#define SPHERICAL_HEX_HEX_GRID_H

#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/basis.hpp>
#include <unordered_map>
#include <vector>
#include "hex_coord.h"
#include "icosahedron.h"

struct HexGridSettings {
    float hex_size = 0.1f;
    int grid_radius = 1;
};

struct FaceGridData {
    int face_index;
    IcosahedronFace face;
    std::vector<HexCoord> local_coords;
    godot::Vector3 center;
    godot::Basis orientation; // columns: tangent, bitangent, normal
};

struct FaceHexCoord {
    int face_index;
    HexCoord hex;

    FaceHexCoord(int f = 0, const HexCoord &h = HexCoord()) : face_index(f), hex(h) {}

    bool operator==(const FaceHexCoord &o) const {
        return face_index == o.face_index && hex == o.hex;
    }
};

struct FaceHexCoordHash {
    std::size_t operator()(const FaceHexCoord &fhc) const noexcept {
        std::size_t h1 = std::hash<int>{}(fhc.face_index);
        HexCoordHash hch;
        std::size_t h2 = hch(fhc.hex);
        return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1<<6) + (h1>>2));
    }
};

class HexGrid {
public:
    explicit HexGrid(const HexGridSettings &s) : settings(s) {}
    void generate_on_face(const IcosahedronFace &face, int face_index);
    std::vector<godot::Vector3> get_neighbor_positions(const FaceHexCoord &coord) const;
    const std::unordered_map<FaceHexCoord, godot::Vector3, FaceHexCoordHash> &get_all_positions() const { return hex_positions; }
private:
    HexGridSettings settings;
    std::unordered_map<FaceHexCoord, godot::Vector3, FaceHexCoordHash> hex_positions;
    std::vector<FaceGridData> face_grids;
};

#endif // SPHERICAL_HEX_HEX_GRID_H
