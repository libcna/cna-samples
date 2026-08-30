// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Cat.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "SpriteEntity.hpp"
#include "System/Random.hpp"
#include "System/TimeSpan.hpp"

namespace Audio3D
{
    /** @brief Entity that moves in a circle and plays single-shot cat sounds. */
    class Cat : public SpriteEntity
    {
    public:
        /**
         * @brief Moves the cat and triggers randomized single-shot sounds.
         * @param gameTime Timing information for the current frame.
         * @param audioManager Manager used to play spatial sounds.
         */
        void Update(
            Microsoft::Xna::Framework::GameTime& gameTime,
            AudioManager& audioManager) override;

    private:
        System::TimeSpan timeDelay = System::TimeSpan::Zero;
        static System::Random random;
    };
}
