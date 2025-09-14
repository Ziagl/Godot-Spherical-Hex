#ifndef SPHERICAL_HEX_ICOSAHEDRON_H
#define SPHERICAL_HEX_ICOSAHEDRON_H

#include <godot_cpp/variant/vector3.hpp>
#include <vector>

struct IcosahedronFace {
    godot::Vector3 v[3];
    godot::Vector3 normal;

    IcosahedronFace() {}
    IcosahedronFace(const godot::Vector3 &a, const godot::Vector3 &b, const godot::Vector3 &c);
    godot::Vector3 center() const;
    std::vector<IcosahedronFace> subdivide(int depth) const;
};

std::vector<IcosahedronFace> generate_icosahedron();

#endif // SPHERICAL_HEX_ICOSAHEDRON_H
