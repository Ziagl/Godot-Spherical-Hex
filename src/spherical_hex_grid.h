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

namespace godot {

class SphericalHexGrid : public Node3D {
    GDCLASS(SphericalHexGrid, Node3D);
public:
    SphericalHexGrid() = default;
    ~SphericalHexGrid() override = default;
    
    void _ready() override;

    void generate_grid();
    void set_radius(double r) { radius = (float)r; }
    void set_resolution(int32_t res) { resolution = res < 0 ? 0 : res; }
    void set_hex_size(double s) { hex_size = (float)s; }
    Object *get_tile_at_position(const Vector3 &world_pos);

protected:
    static void _bind_methods();

private:
    float radius = 1.0f;
    int32_t resolution = 1;
    float hex_size = 0.1f;
    std::vector<IcosahedronFace> faces;
    std::unordered_map<std::string, ObjectID> tiles; // key -> tile id

    Object *create_hex_tile_at(const Vector3 &position, const Vector3 &normal, const HexCoord &coord);
    Object *get_tile_at_world_pos(const Vector3 &pos) const;
};

}

#endif // SPHERICAL_HEX_SPHERICAL_HEX_GRID_H
