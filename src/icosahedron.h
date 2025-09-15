#ifndef SPHERICAL_HEX_ICOSAHEDRON_H
#define SPHERICAL_HEX_ICOSAHEDRON_H

#include <godot_cpp/variant/vector3.hpp>
#include <vector>

using namespace godot;

struct IcosahedronFace {
    Vector3 v[3];
    Vector3 normal;

    IcosahedronFace() {}
    IcosahedronFace(const Vector3 &a, const Vector3 &b, const Vector3 &c);
    Vector3 center() const;
    std::vector<IcosahedronFace> subdivide(int depth) const;
};

std::vector<IcosahedronFace> generate_icosahedron();

#endif // SPHERICAL_HEX_ICOSAHEDRON_H
