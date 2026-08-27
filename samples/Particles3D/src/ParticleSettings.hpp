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
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Settings class describes all the tweakable options used to control the appearance of
     *        a particle system.
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
         *
         * You can see this in action with the explosion effect, where the flames continue to move
         * in the same direction as the source projectile. The projectile trail particles, on the
         * other hand, set this value very low so they are less affected by the projectile.
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
         * upward to make the flames rise, and the smoke plume points it sideways to simulate wind.
         */
        Vector3 Gravity = Vector3::Zero;

        /**
         * @brief Controls how the particle velocity will change over their lifetime.
         *
         * 1 keeps them going at their creation speed, 0 brings them to a complete stop right
         * before they die, and values above 1 make them speed up over time.
         */
        float EndVelocity = 1;

        /** @brief Lower limit of the particle color and alpha. */
        Color MinColor = Color::White;

        /** @brief Upper limit of the particle color and alpha. */
        Color MaxColor = Color::White;

        /**
         * @brief Lower limit of how fast the particles rotate.
         *
         * If both rotate speeds are 0 the particle system automatically switches to an alternative
         * shader technique that does not support rotation, and thus requires significantly less
         * GPU power.
         */
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

        /** @brief Alpha blending settings. */
        BlendState BlendStateValue = BlendState::NonPremultiplied;
    };
}
