// SPDX-License-Identifier: MS-PL

#include "Helpers/RandomHelper.hpp"

#include <cstdint>

#include "System/DateTime.hpp"

namespace RacingGame::Helpers
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;

    System::Random RandomHelper::globalRandomGenerator(
        static_cast<std::int32_t>(
            System::DateTime::getNowProperty().getTicksProperty()));

    System::Random& RandomHelper::GenerateNewRandomGenerator()
    {
        globalRandomGenerator = System::Random(static_cast<std::int32_t>(
            System::DateTime::getNowProperty().getTicksProperty()));
        return globalRandomGenerator;
    }

    int RandomHelper::GetRandomInt(const int max)
    {
        return globalRandomGenerator.Next(max);
    }

    float RandomHelper::GetRandomFloat(const float min, const float max)
    {
        return static_cast<float>(globalRandomGenerator.NextDouble()) *
                   (max - min) +
               min;
    }

    SharpRuntime::bytecs RandomHelper::GetRandomByte(
        const SharpRuntime::bytecs min, const SharpRuntime::bytecs max)
    {
        return static_cast<SharpRuntime::bytecs>(
            globalRandomGenerator.Next(min, max));
    }

    Vector2 RandomHelper::GetRandomVector2(const float min, const float max)
    {
        return {GetRandomFloat(min, max), GetRandomFloat(min, max)};
    }

    Vector3 RandomHelper::GetRandomVector3(const float min, const float max)
    {
        return {GetRandomFloat(min, max), GetRandomFloat(min, max),
                GetRandomFloat(min, max)};
    }

    Color RandomHelper::getRandomColorProperty()
    {
        return Color(Vector3(GetRandomFloat(0.25f, 1.0f),
                             GetRandomFloat(0.25f, 1.0f),
                             GetRandomFloat(0.25f, 1.0f)));
    }

    Vector3 RandomHelper::getRandomNormalVector3Property()
    {
        Vector3 randomNormalVector(GetRandomFloat(-1.0f, 1.0f),
                                   GetRandomFloat(-1.0f, 1.0f),
                                   GetRandomFloat(-1.0f, 1.0f));
        randomNormalVector.Normalize();
        return randomNormalVector;
    }
}
