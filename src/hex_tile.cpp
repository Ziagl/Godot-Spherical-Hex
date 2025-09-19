#include "hex_tile.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>

using namespace godot;

void HexTile::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_neighbors"), &HexTile::get_neighbors);
    ClassDB::bind_method(D_METHOD("get_coordinate"), &HexTile::get_coordinate);
    ClassDB::bind_method(D_METHOD("set_coordinate", "q", "r"), &HexTile::set_coordinate);
    ClassDB::bind_method(D_METHOD("connect_neighbors", "tiles"), &HexTile::connect_neighbors);
}

void HexTile::_ready() {
    Node3D *child = memnew(Node3D);
    Transform3D transform(
        Basis::from_scale(Vector3(0.5, 0.5, 0.5)),
        Vector3(0, 0, 0));
    child->set_transform(transform);
    child->add_to_group("hex_tiles");
    add_child(child);
}

Array HexTile::get_neighbors() {
    Array arr;
    arr.resize(neighbor_ids.size());
    int idx = 0;
    for (auto id : neighbor_ids) {
        Object *obj = ObjectDB::get_instance(id);
        if (obj) arr[idx++] = obj;
    }
    if (idx < neighbor_ids.size()) arr.resize(idx);
    return arr;
}

Vector3 HexTile::get_coordinate() const {
    return { (real_t)coord.q, (real_t)coord.r, (real_t)coord.s };
}

void HexTile::set_coordinate(int32_t q, int32_t r) {
    coord = HexCoord(q, r);
}

void HexTile::connect_neighbors(const Array &tiles) {
    neighbor_ids.clear();
    for (int i = 0; i < tiles.size(); ++i) {
        if (Object *o = Object::cast_to<Node3D>(tiles[i])) {
            neighbor_ids.push_back(o->get_instance_id());
        }
    }
}
