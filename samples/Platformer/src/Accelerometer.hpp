// SPDX-License-Identifier: MS-PL
#pragma once

#include <string>

#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace Platformer
{
    struct AccelerometerState
    {
        AccelerometerState() = default;
        AccelerometerState(Microsoft::Xna::Framework::Vector3 acceleration, bool isActive);

        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getAccelerationProperty() const;
        [[nodiscard]] bool getIsActiveProperty() const;
        [[nodiscard]] std::string ToString() const;

    private:
        Microsoft::Xna::Framework::Vector3 acceleration_;
        bool isActive_ = false;
    };

    class Accelerometer final
    {
    public:
        Accelerometer() = delete;

        static void Initialize();
        [[nodiscard]] static AccelerometerState GetState();
    };
}
