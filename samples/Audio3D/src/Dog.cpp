// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Dog.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Dog.hpp"

namespace Audio3D
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Vector3;

    void Dog::Update(GameTime& gameTime, AudioManager& audioManager)
    {
        setPositionProperty(Vector3(0.0f, 0.0f, -4000.0f));
        setForwardProperty(Vector3::Forward);
        setUpProperty(Vector3::Up);
        setVelocityProperty(Vector3::Zero);

        timeDelay -= gameTime.getElapsedGameTimeProperty();

        if (timeDelay < System::TimeSpan::Zero)
        {
            if (activeSound == nullptr)
            {
                activeSound = audioManager.Play3DSound("DogSound", true, this);
                timeDelay += System::TimeSpan::FromSeconds(6.0);
            }
            else
            {
                activeSound->Stop(false);
                activeSound = nullptr;
                timeDelay += System::TimeSpan::FromSeconds(4.0);
            }
        }
    }
}
