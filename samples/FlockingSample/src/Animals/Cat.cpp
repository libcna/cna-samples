// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Cat.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Animals/Cat.hpp"

#include <optional>

#include "InputState.hpp"

#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"

namespace Flocking
{
    using namespace Microsoft::Xna::Framework::Graphics;

    Cat::Cat(Texture2D tex, int screenWidth, int screenHeight)
        : Animal(tex, screenWidth, screenHeight)
    {
        if (tex.getWidthProperty() > 0)
        {
            texture = tex;
            textureCenter = Vector2((float)(texture.getWidthProperty() / 2),
                                    (float)(texture.getHeightProperty() / 2));
        }
        center.X = (float)(screenWidth / 2);
        center.Y = (float)(screenHeight / 2);
        location = center;
        moveSpeed = 500.0f;
        animaltype = AnimalType::Cat;
    }

    void Cat::Update(const GameTime& gameTime)
    {
        if (direction.Length() > .01f)
        {
            float elapsedTime =
                (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
            location.X += direction.X * moveSpeed * elapsedTime;
            if (location.X < 0.0f)
            {
                location.X = 0.0f;
            }
            else if (location.X > boundryWidth)
            {
                location.X = (float)boundryWidth;
            }
            location.Y += direction.Y * moveSpeed * elapsedTime;
            if (location.Y < 0.0f)
            {
                location.Y = 0.0f;
            }
            else if (location.Y > boundryHeight)
            {
                location.Y = (float)boundryHeight;
            }
        }
    }

    void Cat::Draw(SpriteBatch& spriteBatch, const GameTime& gameTime)
    {
        (void)gameTime;
        spriteBatch.Draw(texture, location, std::nullopt, color,
            0.0f, textureCenter, 1.0f, SpriteEffects::None, 0.0f);
    }

    void Cat::HandleInput(const InputState& input)
    {
        direction.X = input.getMoveCatXProperty();
        direction.Y = input.getMoveCatYProperty();
    }
}
