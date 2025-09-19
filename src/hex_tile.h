#ifndef SPHERICAL_HEX_HEX_TILE_H
#define SPHERICAL_HEX_HEX_TILE_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/core/binder_common.hpp>
#include "hex_coord.h"

using namespace godot;

class HexTile : public Node3D {
    GDCLASS(HexTile, Node3D);
public:
    HexTile() = default;
    ~HexTile() override = default;

    Array get_neighbors();
    Vector3 get_coordinate() const;
    void set_coordinate(int32_t q, int32_t r);
    void connect_neighbors(const Array &tiles);

    void _ready() override;

protected:
    static void _bind_methods();

private:
    HexCoord coord;
    godot::Vector<uint64_t> neighbor_ids; // store by id to avoid strong refs
};

#endif // SPHERICAL_HEX_HEX_TILE_H
