// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Platformer
{
    struct Circle
    {
        Microsoft::Xna::Framework::Vector2 Center;
        float Radius;

        Circle(Microsoft::Xna::Framework::Vector2 position, float radius);
        [[nodiscard]] bool Intersects(Microsoft::Xna::Framework::Rectangle rectangle) const;
    };
}
