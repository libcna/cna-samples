// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ExplosionParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ExplosionParticleSystem.hpp"

namespace Particle3DSample
{
    const std::string& ExplosionParticleSystem::GetTypeName() const
    {
        static const std::string typeName{"Particle3DSample.ExplosionParticleSystem"};
        return typeName;
    }

    void ExplosionParticleSystem::InitializeSettings(ParticleSettings& settings)
    {
        settings.TextureName = "explosion";

        settings.MaxParticles = 100;

        settings.Duration = System::TimeSpan::FromSeconds(2);
        settings.DurationRandomness = 1;

        settings.MinHorizontalVelocity = 20;
        settings.MaxHorizontalVelocity = 30;

        settings.MinVerticalVelocity = -20;
        settings.MaxVerticalVelocity = 20;

        settings.EndVelocity = 0;

        settings.MinColor = Color::DarkGray;
        settings.MaxColor = Color::Gray;

        settings.MinRotateSpeed = -1;
        settings.MaxRotateSpeed = 1;

        settings.MinStartSize = 7;
        settings.MaxStartSize = 7;

        settings.MinEndSize = 70;
        settings.MaxEndSize = 140;

        // Use additive blending.
        settings.BlendStateValue = BlendState::Additive;
    }
}
