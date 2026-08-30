// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Dog.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "SpriteEntity.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"
#include "System/TimeSpan.hpp"

namespace Audio3D
{
    /** @brief Stationary entity that alternates a looping dog sound with silence. */
    class Dog : public SpriteEntity
    {
    public:
        /**
         * @brief Positions the dog and starts or stops its looping sound on schedule.
         * @param gameTime Timing information for the current frame.
         * @param audioManager Manager used to play spatial sounds.
         */
        void Update(
            Microsoft::Xna::Framework::GameTime& gameTime,
            AudioManager& audioManager) override;

    private:
        System::TimeSpan timeDelay = System::TimeSpan::Zero;
        Microsoft::Xna::Framework::Audio::SoundEffectInstance* activeSound = nullptr;
    };
}
