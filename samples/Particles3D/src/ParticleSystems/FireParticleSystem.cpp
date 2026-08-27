// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// FireParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "FireParticleSystem.hpp"

namespace Particle3DSample
{
    const std::string& FireParticleSystem::GetTypeName() const
    {
        static const std::string typeName{"Particle3DSample.FireParticleSystem"};
        return typeName;
    }

    void FireParticleSystem::InitializeSettings(ParticleSettings& settings)
    {
        settings.TextureName = "fire";

        settings.MaxParticles = 2400;

        settings.Duration = System::TimeSpan::FromSeconds(2);

        settings.DurationRandomness = 1;

        settings.MinHorizontalVelocity = 0;
        settings.MaxHorizontalVelocity = 15;

        settings.MinVerticalVelocity = -10;
        settings.MaxVerticalVelocity = 10;

        // Set gravity upside down, so the flames will 'fall' upward.
        settings.Gravity = Vector3(0, 15, 0);

        settings.MinColor = Color(255, 255, 255, 10);
        settings.MaxColor = Color(255, 255, 255, 40);

        settings.MinStartSize = 5;
        settings.MaxStartSize = 10;

        settings.MinEndSize = 10;
        settings.MaxEndSize = 40;

        // Use additive blending.
        settings.BlendStateValue = BlendState::Additive;
    }
}
