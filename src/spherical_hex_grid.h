#ifndef SPHERICAL_HEX_SPHERICAL_HEX_GRID_H
#define SPHERICAL_HEX_SPHERICAL_HEX_GRID_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <unordered_map>
#include <string>
#include "icosahedron.h"
#include "hex_grid.h"
#include "hex_tile.h"

class SphericalHexGrid : public godot::Node3D {
    GDCLASS(SphericalHexGrid, godot::Node3D);
public:
    SphericalHexGrid() = default;
    ~SphericalHexGrid() override = default;

    void generate_grid();
    void set_radius(double r) { radius = (float)r; }
    void set_resolution(int32_t res) { resolution = res < 0 ? 0 : res; }
    void set_hex_size(double s) { hex_size = (float)s; }
    godot::Object *get_tile_at_position(const godot::Vector3 &world_pos);

//protected:
    static void _bind_methods();
    void _ready() override;

private:
    float radius = 1.0f;
    int32_t resolution = 1;
    float hex_size = 0.1f;
    std::vector<IcosahedronFace> faces;
    std::unordered_map<std::string, godot::ObjectID> tiles; // key -> tile id

    godot::Object *create_hex_tile_at(const godot::Vector3 &position, const godot::Vector3 &normal, const HexCoord &coord);
    godot::Object *get_tile_at_world_pos(const godot::Vector3 &pos) const;
};

#endif // SPHERICAL_HEX_SPHERICAL_HEX_GRID_H
