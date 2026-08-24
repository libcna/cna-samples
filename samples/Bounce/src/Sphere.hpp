// SPDX-License-Identifier: MS-PL
#pragma once
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace Bounce {

using namespace Microsoft::Xna::Framework;

class Sphere {
public:
    Vector3 Position;
    Vector3 Velocity;
    float   Radius      = 0.0f;
    float   Mass        = 0.0f;
    Microsoft::Xna::Framework::Color Color{0, 0, 0, 0};
};

} // namespace Bounce
