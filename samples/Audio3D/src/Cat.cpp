// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Cat.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Cat.hpp"

#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Int32.hpp"
#include "System/Math.hpp"

namespace Audio3D
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Vector3;
    using SharpRuntime::Double;
    using SharpRuntime::Single;
    using SharpRuntime::String;

    System::Random Cat::random;

    void Cat::Update(GameTime& gameTime, AudioManager& audioManager)
    {
        const Double time = gameTime.getTotalGameTimeProperty().getTotalSecondsProperty();

        const Single dx = static_cast<Single>(-System::Math::Cos(time));
        const Single dz = static_cast<Single>(-System::Math::Sin(time));

        const Vector3 newPosition = Vector3(dx, 0.0f, dz) * 6000.0f;

        setVelocityProperty(newPosition - getPositionProperty());
        setPositionProperty(newPosition);
        if (getVelocityProperty() == Vector3::Zero)
        {
            setForwardProperty(Vector3::Forward);
        }
        else
        {
            setForwardProperty(Vector3::Normalize(getVelocityProperty()));
        }

        setUpProperty(Vector3::Up);

        timeDelay -= gameTime.getElapsedGameTimeProperty();

        if (timeDelay < System::TimeSpan::Zero)
        {
            const String soundName = "CatSound" + System::Int32::ToString(random.Next(3));
            audioManager.Play3DSound(soundName, false, this);
            timeDelay += System::TimeSpan::FromSeconds(1.25f);
        }
    }
}
