// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ProjectileTrailParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "ProjectileTrailParticleSystem.hpp"

namespace Particle3DSample
{
    const std::string& ProjectileTrailParticleSystem::GetTypeName() const
    {
        static const std::string typeName{"Particle3DSample.ProjectileTrailParticleSystem"};
        return typeName;
    }

    void ProjectileTrailParticleSystem::InitializeSettings(ParticleSettings& settings)
    {
        settings.TextureName = "smoke";

        settings.MaxParticles = 1000;

        settings.Duration = System::TimeSpan::FromSeconds(3);

        settings.DurationRandomness = 1.5f;

        settings.EmitterVelocitySensitivity = 0.1f;

        settings.MinHorizontalVelocity = 0;
        settings.MaxHorizontalVelocity = 1;

        settings.MinVerticalVelocity = -1;
        settings.MaxVerticalVelocity = 1;

        settings.MinColor = Color(64, 96, 128, 255);
        settings.MaxColor = Color(255, 255, 255, 128);

        settings.MinRotateSpeed = -4;
        settings.MaxRotateSpeed = 4;

        settings.MinStartSize = 1;
        settings.MaxStartSize = 3;

        settings.MinEndSize = 4;
        settings.MaxEndSize = 11;
    }
}
