// SPDX-License-Identifier: MS-PL

#include "Tile.hpp"

#include <utility>

namespace Platformer
{
    Tile::Tile(std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> texture,
               const TileCollision collision)
        : Texture(std::move(texture)), Collision(collision)
    {
    }
}
