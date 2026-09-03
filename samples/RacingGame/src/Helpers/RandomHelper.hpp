// SPDX-License-Identifier: MS-PL

#pragma once

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Random.hpp"

namespace RacingGame::Helpers
{
    /** @brief Provides the original game-wide pseudo-random number generator. */
    class RandomHelper final
    {
    public:
        /** @brief Global generator shared by all Racing subsystems. */
        static System::Random globalRandomGenerator;

        /**
         * @brief Reseeds the global generator from the current clock.
         * @return Reference to the newly seeded generator.
         */
        static System::Random& GenerateNewRandomGenerator();

        /**
         * @brief Gets a random integer below an exclusive maximum.
         * @param max Exclusive upper bound.
         * @return Random integer in the range `[0,max)`.
         */
        [[nodiscard]] static int GetRandomInt(int max);

        /**
         * @brief Gets a random floating-point value in a range.
         * @param min Inclusive lower bound.
         * @param max Upper bound.
         * @return Random value between the supplied bounds.
         */
        [[nodiscard]] static float GetRandomFloat(float min, float max);

        /**
         * @brief Gets a random byte in a range.
         * @param min Inclusive lower bound.
         * @param max Exclusive upper bound.
         * @return Random byte between the supplied bounds.
         */
        [[nodiscard]] static SharpRuntime::bytecs GetRandomByte(
            SharpRuntime::bytecs min, SharpRuntime::bytecs max);

        /**
         * @brief Gets a two-dimensional vector with random components.
         * @param min Minimum component value.
         * @param max Maximum component value.
         * @return Random vector.
         */
        [[nodiscard]] static Microsoft::Xna::Framework::Vector2
        GetRandomVector2(float min, float max);

        /**
         * @brief Gets a three-dimensional vector with random components.
         * @param min Minimum component value.
         * @param max Maximum component value.
         * @return Random vector.
         */
        [[nodiscard]] static Microsoft::Xna::Framework::Vector3
        GetRandomVector3(float min, float max);

        /** @brief Gets a random bright color. */
        [[nodiscard]] static Microsoft::Xna::Framework::Color
        getRandomColorProperty();

        /** @brief Gets a random normalized three-dimensional vector. */
        [[nodiscard]] static Microsoft::Xna::Framework::Vector3
        getRandomNormalVector3Property();

    private:
        RandomHelper() = delete;
    };
}
