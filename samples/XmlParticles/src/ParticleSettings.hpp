// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleSettings.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "System/TimeSpan.hpp"

namespace Particle3DSample
{
    using namespace Microsoft::Xna::Framework;

    /**
     * @brief Settings class describes all the tweakable options used to control the appearance of
     *        a particle system.
     *
     * Unlike the sibling Particles3D sample, nothing here is written in code: every value arrives
     * from an XML file the content pipeline compiled. See `ParticleSettingsReader.hpp` for the
     * order the pipeline writes them in.
     */
    class ParticleSettings
    {
    public:
        /** @brief Name of the texture used by this particle system. */
        std::string TextureName;

        /** @brief Maximum number of particles that can be displayed at one time. */
        int MaxParticles = 100;

        /** @brief How long these particles will last. */
        System::TimeSpan Duration = System::TimeSpan::FromSeconds(1);

        /** @brief If greater than zero, some particles will last a shorter time than others. */
        float DurationRandomness = 0;

        /**
         * @brief Controls how much particles are influenced by the velocity of the object which
         *        created them.
         */
        float EmitterVelocitySensitivity = 1;

        /** @brief Lower limit of the X and Z axis velocity given to each particle. */
        float MinHorizontalVelocity = 0;

        /** @brief Upper limit of the X and Z axis velocity given to each particle. */
        float MaxHorizontalVelocity = 0;

        /** @brief Lower limit of the Y axis velocity given to each particle. */
        float MinVerticalVelocity = 0;

        /** @brief Upper limit of the Y axis velocity given to each particle. */
        float MaxVerticalVelocity = 0;

        /**
         * @brief Direction and strength of the gravity effect.
         *
         * Note that this can point in any direction, not just down: the fire effect points it
         * upward to make the flames rise, and the smoke plume points it sideways to make wind.
         */
        Vector3 Gravity = Vector3::Zero;

        /** @brief Controls how the particle velocity will change over their lifetime. */
        float EndVelocity = 1;

        /** @brief Lower limit of the particle color and alpha. */
        Color MinColor = Color::White;

        /** @brief Upper limit of the particle color and alpha. */
        Color MaxColor = Color::White;

        /** @brief Lower limit of how fast the particles rotate. */
        float MinRotateSpeed = 0;

        /** @brief Upper limit of how fast the particles rotate. */
        float MaxRotateSpeed = 0;

        /** @brief Lower limit of how big the particles are when first created. */
        float MinStartSize = 100;

        /** @brief Upper limit of how big the particles are when first created. */
        float MaxStartSize = 100;

        /** @brief Lower limit of how big particles become at the end of their life. */
        float MinEndSize = 100;

        /** @brief Upper limit of how big particles become at the end of their life. */
        float MaxEndSize = 100;

        /**
         * @brief Alpha blending settings.
         *
         * The field carries the same name as its type, as XNA's does; C++ allows it when the type
         * is written fully qualified at that point. The XML does not store this object -- the
         * original marks it `[ContentSerializerIgnore]` and serializes a private string property
         * beside it instead.
         */
        ::Microsoft::Xna::Framework::Graphics::BlendState BlendState =
            ::Microsoft::Xna::Framework::Graphics::BlendState::NonPremultiplied;
    };
}
