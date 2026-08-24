// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>

#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Platformer
{
    enum class TileCollision
    {
        Passable = 0,
        Impassable = 1,
        Platform = 2,
    };

    struct Tile
    {
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> Texture;
        TileCollision Collision = TileCollision::Passable;

        static constexpr int Width = 40;
        static constexpr int Height = 32;
        inline static const Microsoft::Xna::Framework::Vector2 Size{
            static_cast<float>(Width), static_cast<float>(Height)};

        Tile() = default;
        Tile(std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture,
             TileCollision collision);
    };
}
