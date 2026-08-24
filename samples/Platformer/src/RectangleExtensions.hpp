// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Platformer
{
    class RectangleExtensions final
    {
    public:
        RectangleExtensions() = delete;

        [[nodiscard]] static Microsoft::Xna::Framework::Vector2 GetIntersectionDepth(
            Microsoft::Xna::Framework::Rectangle rectA,
            Microsoft::Xna::Framework::Rectangle rectB);

        [[nodiscard]] static Microsoft::Xna::Framework::Vector2 GetBottomCenter(
            Microsoft::Xna::Framework::Rectangle rect);
    };
}
