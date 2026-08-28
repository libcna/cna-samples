// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Animation.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Animation.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"

namespace Graphics3DSample
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Rectangle;

    Animation::Animation(const Texture2D& frameSheet, const Point& size,
                         const Point& frameSheetSize, const System::TimeSpan& interval)
        : animationTexture(frameSheet), sheetSize(frameSheetSize), frameInterval(interval)
    {
        frameSize = size;
    }

    bool Animation::Update(const GameTime& gameTime)
    {
        bool progressed;

        // Check is it is a time to progress to the next frame
        if (nextFrame >= frameInterval)
        {
            // Progress to the next frame in the row
            currentFrame.X++;
            // If reached end of the row advance to the next row
            // and start form the first frame there
            if (currentFrame.X >= sheetSize.X)
            {
                currentFrame.X = 0;
                currentFrame.Y++;
            }
            // If reached last row in the frame sheet jump to the first row again - produce
            // endless loop
            if (currentFrame.Y >= sheetSize.Y)
            {
                currentFrame.Y = 0;
            }

            // Reset interval for next frame
            progressed = true;
            nextFrame = System::TimeSpan::Zero;
        }
        else
        {
            // Wait for the next frame
            nextFrame = nextFrame + gameTime.getElapsedGameTimeProperty();
            progressed = false;
        }

        return progressed;
    }

    void Animation::Draw(SpriteBatch& spriteBatch, const Vector2& position,
                         SpriteEffects spriteEffect)
    {
        Draw(spriteBatch, position, 1.0f, spriteEffect);
    }

    void Animation::Draw(SpriteBatch& spriteBatch, const Vector2& position, float scale,
                         SpriteEffects spriteEffect)
    {
        spriteBatch.Draw(animationTexture, position,
                         Rectangle(frameSize.X * currentFrame.X,
                                   frameSize.Y * currentFrame.Y,
                                   frameSize.X,
                                   frameSize.Y),
                         Color::White, 0.0f, Vector2::Zero, scale, spriteEffect, 0);
    }
}
