#include "spherical_hex_grid.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace godot;

static inline std::string coord_key(int face_index, const HexCoord &c) {
    return std::to_string(face_index) + ":" + std::to_string(c.q) + "_" + std::to_string(c.r);
}

static inline Vector3 face_normal_at_point(const Vector3 &p) { return p.normalized(); }

void SphericalHexGrid::_bind_methods() {
    ClassDB::bind_method(D_METHOD("generate_grid"), &SphericalHexGrid::generate_grid);
    ClassDB::bind_method(D_METHOD("get_radius"), &SphericalHexGrid::get_radius);
    ClassDB::bind_method(D_METHOD("get_resolution"), &SphericalHexGrid::get_resolution);
    ClassDB::bind_method(D_METHOD("get_hex_size"), &SphericalHexGrid::get_hex_size);
    ClassDB::bind_method(D_METHOD("set_radius", "r"), &SphericalHexGrid::set_radius);
    ClassDB::bind_method(D_METHOD("set_resolution", "res"), &SphericalHexGrid::set_resolution);
    ClassDB::bind_method(D_METHOD("set_hex_size", "size"), &SphericalHexGrid::set_hex_size);
    ClassDB::bind_method(D_METHOD("get_tile_at_position", "world_pos"), &SphericalHexGrid::get_tile_at_position);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "radius"), "set_radius", "get_radius");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "resolution"), "set_resolution", "get_resolution");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "hex_size"), "set_hex_size", "get_hex_size");
}

void SphericalHexGrid::_ready() {
    generate_grid();
}

Object *SphericalHexGrid::create_hex_tile_at(const Vector3 &position, const Vector3 &normal, int face_index, const HexCoord &coord) {
    HexTile *tile = memnew(HexTile);
    add_child(tile);

    // Orientation basis
    Vector3 up(0,1,0);
    Basis basis;
    float dot = normal.dot(up);
    if (dot > 0.999f) {
        basis = Basis();
    } else if (dot < -0.999f) {
        basis = Basis().rotated(Vector3(1,0,0), (real_t)M_PI);
    } else {
        Vector3 right = up.cross(normal).normalized();
        Vector3 upv = normal.cross(right);
        basis = Basis(right, upv, normal);
    }

    tile->set_transform(Transform3D(basis, position * radius));
    tile->set_coordinate(coord.q, coord.r);

    tiles[coord_key(face_index, coord)] = tile->get_instance_id();
    return tile;
}

Object *SphericalHexGrid::get_tile_at_world_pos(const Vector3 &pos) const {
    // naive nearest search
    Object *best = nullptr; float best_d2 = 1e30f;
    for (auto &kv : tiles) {
        Object *obj = ObjectDB::get_instance(kv.second);
        if (!obj) continue;
        if (HexTile *tile = Object::cast_to<HexTile>(obj)) {
            Vector3 tpos = tile->get_position();
            float d2 = (tpos - pos).length_squared();
            if (d2 < best_d2) { best_d2 = d2; best = tile; }
        }
    }
    return best;
}

void SphericalHexGrid::generate_grid() {
    // Clear existing
    for (auto &kv : tiles) {
        if (Object *o = ObjectDB::get_instance(kv.second)) {
            if (Node *n = Object::cast_to<Node>(o)) n->queue_free();
        }
    }
    tiles.clear();

    faces.clear();
    auto base = generate_icosahedron();
    // Use base icosahedron faces; density is controlled via grid_radius
    for (auto &f : base) {
        faces.push_back(f);
    }

    HexGridSettings settings{ hex_size, (int)(6.0 * std::pow(2.0, resolution)) };
    HexGrid hex_grid(settings);

    for (size_t i = 0; i < faces.size(); ++i) {
        hex_grid.generate_on_face(faces[i], (int)i);
    }

    // Create tiles (deduplicate by position to avoid overlaps at face borders)
    std::unordered_map<std::string, ObjectID> pos_to_id;
    std::vector<std::pair<FaceHexCoord, std::string>> created_entries;
    auto &all = hex_grid.get_all_positions();
    for (auto it = all.begin(); it != all.end(); ++it) {
        Vector3 p = it->second.normalized();
        // key by quantized position
        int kx = (int)std::round(p.x * 100000.0f);
        int ky = (int)std::round(p.y * 100000.0f);
        int kz = (int)std::round(p.z * 100000.0f);
        std::string pk = std::to_string(kx) + "," + std::to_string(ky) + "," + std::to_string(kz);
        if (pos_to_id.find(pk) != pos_to_id.end()) continue;
        Vector3 n = face_normal_at_point(p);
        if (Object *obj = create_hex_tile_at(p, n, it->first.face_index, it->first.hex)) {
            pos_to_id[pk] = Object::cast_to<Node>(obj)->get_instance_id();
            created_entries.emplace_back(it->first, pk);
        }
    }

    // Connect neighbors
    // Link neighbors using hash lookups to avoid O(N^2) nearest searches
    for (auto &pair : created_entries) {
        const FaceHexCoord &fhc = pair.first;
        auto neighbors_pos = hex_grid.get_neighbor_positions(fhc);
        Array neighbor_objs;
        for (auto &npos : neighbors_pos) {
            Vector3 np = npos.normalized();
            int nkx = (int)std::round(np.x * 100000.0f);
            int nky = (int)std::round(np.y * 100000.0f);
            int nkz = (int)std::round(np.z * 100000.0f);
            std::string npk = std::to_string(nkx) + "," + std::to_string(nky) + "," + std::to_string(nkz);
            auto it_id = pos_to_id.find(npk);
            if (it_id != pos_to_id.end()) {
                if (Object *o = ObjectDB::get_instance(it_id->second)) neighbor_objs.append(o);
            }
        }
        auto it_self = pos_to_id.find(pair.second);
        if (it_self != pos_to_id.end()) {
            if (Object *obj = ObjectDB::get_instance(it_self->second)) {
                if (HexTile *tile = Object::cast_to<HexTile>(obj)) tile->connect_neighbors(neighbor_objs);
            }
        }
    }
}

Object *SphericalHexGrid::get_tile_at_position(const Vector3 &world_pos) {
    return get_tile_at_world_pos(world_pos);
}
