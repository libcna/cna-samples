// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Ship.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace ChaseCameraSample
{
    using SharpRuntime::Single;

    /** @brief Simulates and orients the ship followed by the chase camera. */
    class Ship
    {
    public:
        /** @brief Location of the ship in world space. */
        Microsoft::Xna::Framework::Vector3 Position;

        /** @brief Direction the ship is facing. */
        Microsoft::Xna::Framework::Vector3 Direction;

        /** @brief Ship's up vector. */
        Microsoft::Xna::Framework::Vector3 Up;

        /**
         * @brief Gets the ship's right vector.
         * @return The ship's right vector.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getRightProperty() const;

        /** @brief Current ship velocity. */
        Microsoft::Xna::Framework::Vector3 Velocity;

        /**
         * @brief Gets the ship's world transform matrix.
         * @return The ship's world transform matrix.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix getWorldProperty() const;

        /**
         * @brief Constructs a ship that uses the device viewport for mouse touch regions.
         * @param device Graphics device whose viewport defines the touch regions.
         */
        explicit Ship(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);

        /** @brief Restores the ship to its original starting state. */
        void Reset();

        /**
         * @brief Applies steering, thrust, drag, and position integration.
         * @param gameTime Timing information for the current frame.
         */
        void Update(const Microsoft::Xna::Framework::GameTime& gameTime);

    private:
        static constexpr Single MinimumAltitude = 350.0f;
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphicsDevice;

        Microsoft::Xna::Framework::Vector3 right;

        static constexpr Single RotationRate = 1.5f;
        static constexpr Single Mass = 1.0f;
        static constexpr Single ThrustForce = 24000.0f;
        static constexpr Single DragFactor = 0.97f;

        Microsoft::Xna::Framework::Matrix world;

        [[nodiscard]] bool TouchLeft() const;
        [[nodiscard]] bool TouchRight() const;
        [[nodiscard]] bool TouchDown() const;
        [[nodiscard]] bool TouchUp() const;
    };
}
