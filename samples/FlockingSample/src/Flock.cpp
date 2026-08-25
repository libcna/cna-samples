// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Flock.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Flock.hpp"

#include "System/Random.hpp"

namespace Flocking
{
    AIParameters Flock::getFlockParamsProperty() const
    {
        // Upstream: `get { return FlockParams; }`. Preserved verbatim; see the header.
        return getFlockParamsProperty();
    }

    Flock::Flock(Texture2D tex, int screenWidth, int screenHeight,
                 const AIParameters& flockParameters)
    {
        boundryWidth = screenWidth;
        boundryHeight = screenHeight;
        birdTexture = tex;
        flockParams = flockParameters;
        ResetFlock();
    }

    void Flock::Update(const GameTime& gameTime, Cat* cat)
    {
        for (Bird& thisBird : flock)
        {
            thisBird.ResetThink();
            for (Bird& otherBird : flock)
            {
                // this check is so we don't try to fly to ourself!
                if (&thisBird != &otherBird)
                {
                    thisBird.ReactTo(&otherBird, flockParams);
                }
            }
            // Look for the cat
            thisBird.ReactTo(cat, flockParams);
            thisBird.Update(gameTime, flockParams);
        }
    }

    void Flock::Draw(SpriteBatch& spriteBatch, const GameTime& gameTime)
    {
        for (Bird& theBird : flock)
        {
            theBird.Draw(spriteBatch, gameTime);
        }
    }

    void Flock::ResetFlock()
    {
        flock.clear();
        // The original's `flock.Capacity = flockSize` is a reserve, and here it is
        // load-bearing rather than an optimisation: each bird's behaviors hold a pointer
        // back to it, so the storage must not reallocate while the flock is being filled.
        flock.reserve(flockSize);
        Vector2 tempDir;
        Vector2 tempLoc;
        System::Random random;

        for (int i = 0; i < flockSize; i++)
        {
            tempLoc = Vector2((float)random.Next(boundryWidth),
                              (float)random.Next(boundryHeight));
            tempDir = Vector2((float)random.NextDouble() - 0.5f,
                              (float)random.NextDouble() - 0.5f);
            tempDir.Normalize();
            flock.emplace_back(birdTexture, tempDir, tempLoc,
                               boundryWidth, boundryHeight);
        }
    }
}
