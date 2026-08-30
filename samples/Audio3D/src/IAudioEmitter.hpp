// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// IAudioEmitter.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace Audio3D
{
    /** @brief Provides the position and velocity of an entity that can emit 3D sounds. */
    class IAudioEmitter
    {
    public:
        /** @brief Destroys the emitter interface. */
        virtual ~IAudioEmitter() = default;

        /**
         * @brief Gets the emitter position.
         * @return The emitter position in world space.
         */
        [[nodiscard]] virtual Microsoft::Xna::Framework::Vector3 getPositionProperty() const = 0;

        /**
         * @brief Gets the direction the emitter is facing.
         * @return The emitter forward vector.
         */
        [[nodiscard]] virtual Microsoft::Xna::Framework::Vector3 getForwardProperty() const = 0;

        /**
         * @brief Gets the emitter orientation.
         * @return The emitter up vector.
         */
        [[nodiscard]] virtual Microsoft::Xna::Framework::Vector3 getUpProperty() const = 0;

        /**
         * @brief Gets the emitter velocity.
         * @return The emitter velocity in world units per update.
         */
        [[nodiscard]] virtual Microsoft::Xna::Framework::Vector3 getVelocityProperty() const = 0;
    };
}
