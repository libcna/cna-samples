// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Animal.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Animals/Animal.hpp"

#include <optional>

#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "System/Math.hpp"

namespace Flocking
{
    using namespace Microsoft::Xna::Framework::Graphics;

    Animal::Animal(Texture2D tex, int screenWidth, int screenHeight)
    {
        if (!tex.getIsDisposedProperty() && tex.getWidthProperty() > 0)
        {
            texture = tex;
            textureCenter = Vector2((float)(texture.getWidthProperty() / 2),
                                    (float)(texture.getHeightProperty() / 2));
        }
        boundryWidth = screenWidth;
        boundryHeight = screenHeight;
        moveSpeed = 0.0f;
    }

    void Animal::Update(const GameTime& gameTime)
    {
        (void)gameTime;
    }

    void Animal::Draw(SpriteBatch& spriteBatch, const GameTime& gameTime)
    {
        (void)gameTime;
        float rotation = (float)System::Math::Atan2(direction.Y, direction.X);
        spriteBatch.Draw(texture, location, std::nullopt, color,
            rotation, textureCenter, 1.0f, SpriteEffects::None, 0.0f);
    }
}
