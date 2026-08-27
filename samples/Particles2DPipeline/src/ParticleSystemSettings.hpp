// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ParticleSystemSettings.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/Blend.hpp"

namespace ParticlesSettings
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /** @brief How a particle system computes each particle's acceleration. */
    enum class AccelerationMode
    {
        /** @brief Particles are not accelerated at all. */
        None,
        /** @brief Acceleration is a scalar along the particle's own direction. */
        Scalar,
        /** @brief Acceleration is derived from a desired end velocity. */
        EndVelocity,
        /** @brief Acceleration is a vector chosen between two limits. */
        Vector
    };

    /**
     * @brief Everything the content pipeline knows about one particle system.
     *
     * The field ORDER is part of this type's contract: the settings arrive from a `.xnb` written
     * by XNA's `IntermediateSerializer`, which writes public fields in declaration order, and
     * `ParticleSystemSettingsReader` reads them back in exactly that order.
     */
    class ParticleSystemSettings
    {
    public:
        /** @brief Lower limit of how many particles one burst creates. */
        int MinNumParticles = 0;

        /** @brief Upper limit of how many particles one burst creates. */
        int MaxNumParticles = 0;

        /** @brief Name of the texture asset the particles are drawn with. */
        std::string TextureFilename;

        /** @brief Lower limit of the direction each particle is fired in, in degrees. */
        float MinDirectionAngle = 0;

        /** @brief Upper limit of the direction each particle is fired in, in degrees. */
        float MaxDirectionAngle = 360;

        /** @brief Lower limit of a particle's initial speed. */
        float MinInitialSpeed = 0;

        /** @brief Upper limit of a particle's initial speed. */
        float MaxInitialSpeed = 0;

        /**
         * @brief Which of the four acceleration models this system uses.
         *
         * The field carries the same name as its type, as XNA's does. C++ allows it -- the
         * declaration hides the type name for the rest of the class, which nothing here needs.
         */
        ::ParticlesSettings::AccelerationMode AccelerationMode =
            ::ParticlesSettings::AccelerationMode::None;

        /** @brief The speed a particle should reach at the end of its life, as a fraction. */
        float EndVelocity = 1.0f;

        /** @brief Lower limit of the scalar acceleration, when the mode is Scalar. */
        float MinAccelerationScale = 0;

        /** @brief Upper limit of the scalar acceleration, when the mode is Scalar. */
        float MaxAccelerationScale = 0;

        /** @brief Lower limit of the acceleration vector, when the mode is Vector. */
        Vector2 MinAccelerationVector = Vector2::Zero;

        /** @brief Upper limit of the acceleration vector, when the mode is Vector. */
        Vector2 MaxAccelerationVector = Vector2::Zero;

        /** @brief How much of the emitting object's velocity each particle inherits. */
        float EmitterVelocitySensitivity = 0;

        /** @brief Lower limit of how fast a particle spins, in degrees per second. */
        float MinRotationSpeed = 0;

        /** @brief Upper limit of how fast a particle spins, in degrees per second. */
        float MaxRotationSpeed = 0;

        /** @brief Lower limit of how long a particle lives, in seconds. */
        float MinLifetime = 0;

        /** @brief Upper limit of how long a particle lives, in seconds. */
        float MaxLifetime = 0;

        /** @brief Lower limit of a particle's scale. */
        float MinSize = 1;

        /** @brief Upper limit of a particle's scale. */
        float MaxSize = 1;

        /** @brief Constant acceleration applied to every particle. */
        Vector2 Gravity = Vector2::Zero;

        /** @brief Source blend factor the system draws with. */
        Blend SourceBlend = Blend::One;

        /** @brief Destination blend factor the system draws with. */
        Blend DestinationBlend = Blend::InverseSourceAlpha;
    };
}
