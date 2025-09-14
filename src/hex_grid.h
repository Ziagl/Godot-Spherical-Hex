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
    IcosahedronFace face;
    std::vector<HexCoord> local_coords;
    godot::Vector3 center;
    godot::Basis orientation; // columns: tangent, bitangent, normal
};

class HexGrid {
public:
    explicit HexGrid(const HexGridSettings &s) : settings(s) {}
    void generate_on_face(const IcosahedronFace &face);
    std::vector<godot::Vector3> get_neighbor_positions(const HexCoord &coord) const;
    const std::unordered_map<HexCoord, godot::Vector3, HexCoordHash> &get_all_positions() const { return hex_positions; }
private:
    HexGridSettings settings;
    std::unordered_map<HexCoord, godot::Vector3, HexCoordHash> hex_positions;
    std::vector<FaceGridData> face_grids;
};

#endif // SPHERICAL_HEX_HEX_GRID_H
