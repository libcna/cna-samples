// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SmokePlumeParticleSystem.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "SmokePlumeParticleSystem.hpp"

namespace Particle3DSample
{
    const std::string& SmokePlumeParticleSystem::GetTypeName() const
    {
        static const std::string typeName{"Particle3DSample.SmokePlumeParticleSystem"};
        return typeName;
    }

    void SmokePlumeParticleSystem::InitializeSettings(ParticleSettings& settings)
    {
        settings.TextureName = "smoke";

        settings.MaxParticles = 600;

        settings.Duration = System::TimeSpan::FromSeconds(10);

        settings.MinHorizontalVelocity = 0;
        settings.MaxHorizontalVelocity = 15;

        settings.MinVerticalVelocity = 10;
        settings.MaxVerticalVelocity = 20;

        // Create a wind effect by tilting the gravity vector sideways.
        settings.Gravity = Vector3(-20, -5, 0);

        settings.EndVelocity = 0.75f;

        settings.MinRotateSpeed = -1;
        settings.MaxRotateSpeed = 1;

        settings.MinStartSize = 4;
        settings.MaxStartSize = 7;

        settings.MinEndSize = 35;
        settings.MaxEndSize = 140;
    }
}
