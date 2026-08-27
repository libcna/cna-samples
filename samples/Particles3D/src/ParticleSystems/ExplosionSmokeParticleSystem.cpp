// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ExplosionSmokeParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ExplosionSmokeParticleSystem.hpp"

namespace Particle3DSample
{
    const std::string& ExplosionSmokeParticleSystem::GetTypeName() const
    {
        static const std::string typeName{"Particle3DSample.ExplosionSmokeParticleSystem"};
        return typeName;
    }

    void ExplosionSmokeParticleSystem::InitializeSettings(ParticleSettings& settings)
    {
        settings.TextureName = "smoke";

        settings.MaxParticles = 200;

        settings.Duration = System::TimeSpan::FromSeconds(4);

        settings.MinHorizontalVelocity = 0;
        settings.MaxHorizontalVelocity = 50;

        settings.MinVerticalVelocity = -10;
        settings.MaxVerticalVelocity = 50;

        settings.Gravity = Vector3(0, -20, 0);

        settings.EndVelocity = 0;

        settings.MinColor = Color::LightGray;
        settings.MaxColor = Color::White;

        settings.MinRotateSpeed = -2;
        settings.MaxRotateSpeed = 2;

        settings.MinStartSize = 7;
        settings.MaxStartSize = 7;

        settings.MinEndSize = 70;
        settings.MaxEndSize = 140;
    }
}
