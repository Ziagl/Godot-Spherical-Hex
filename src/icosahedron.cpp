#include "icosahedron.h"
#include <godot_cpp/variant/utility_functions.hpp>
#include <cmath>

using namespace godot;

static constexpr double PHI = 1.618033988749895;
static constexpr double PHI_NORM = 0.8506508083520399; // 1/sqrt(1 + phi^2)
static constexpr double ONE_NORM = 0.5257311121191336; // 1/sqrt(1 + phi^2)

IcosahedronFace::IcosahedronFace(const Vector3 &a, const Vector3 &b, const Vector3 &c) {
    v[0] = a; v[1] = b; v[2] = c;
    normal = (b - a).cross(c - a).normalized();
}

Vector3 IcosahedronFace::center() const {
    return (v[0] + v[1] + v[2]) / 3.0f; // not normalized yet
}

std::vector<IcosahedronFace> IcosahedronFace::subdivide(int depth) const {
    if (depth == 0) return { *this };
    Vector3 mid1 = (v[0] + v[1]).normalized();
    Vector3 mid2 = (v[1] + v[2]).normalized();
    Vector3 mid3 = (v[2] + v[0]).normalized();

    IcosahedronFace f1(v[0], mid1, mid3);
    IcosahedronFace f2(mid1, v[1], mid2);
    IcosahedronFace f3(mid3, mid2, v[2]);
    IcosahedronFace f4(mid1, mid2, mid3);
    std::vector<IcosahedronFace> out;
    out.reserve(4 * (int)std::pow(4, depth));
    for (auto &f : {f1,f2,f3,f4}) {
        auto sub = f.subdivide(depth - 1);
        out.insert(out.end(), sub.begin(), sub.end());
    }
    return out;
}

std::vector<IcosahedronFace> generate_icosahedron() {
    std::vector<Vector3> verts = {
        { (real_t)ONE_NORM, 0, (real_t)PHI_NORM },
        { (real_t)-ONE_NORM, 0, (real_t)PHI_NORM },
        { (real_t)ONE_NORM, 0, (real_t)-PHI_NORM },
        { (real_t)-ONE_NORM, 0, (real_t)-PHI_NORM },
        { 0, (real_t)PHI_NORM, (real_t)ONE_NORM },
        { 0, (real_t)-PHI_NORM, (real_t)ONE_NORM },
        { 0, (real_t)PHI_NORM, (real_t)-ONE_NORM },
        { 0, (real_t)-PHI_NORM, (real_t)-ONE_NORM },
        { (real_t)PHI_NORM, (real_t)ONE_NORM, 0 },
        { (real_t)-PHI_NORM, (real_t)ONE_NORM, 0 },
        { (real_t)PHI_NORM, (real_t)-ONE_NORM, 0 },
        { (real_t)-PHI_NORM, (real_t)-ONE_NORM, 0 },
    };

    int faces[20][3] = {
        {0,4,8},{0,8,10},{0,10,5},{0,5,1},{0,1,4},
        {4,1,9},{8,4,6},{10,8,2},{5,10,7},{1,5,11},
        {3,6,9},{3,9,11},{3,11,7},{3,7,2},{3,2,6},
        {9,6,4},{6,2,8},{2,7,10},{7,11,5},{11,9,1}
    };

    std::vector<IcosahedronFace> out; out.reserve(20);
    for (auto &fi : faces) {
        out.emplace_back(verts[fi[0]], verts[fi[1]], verts[fi[2]]);
    }
    return out;
}
