// SPDX-License-Identifier: MS-PL
#pragma once

#include "SpacewarSceneItem.hpp"
#include "System/Random.hpp"

namespace Spacewar
{
    enum class AsteroidType
    {
        Large,
        Small,
    };

    class Asteroid final : public SpacewarSceneItem
    {
    public:
        Asteroid(Microsoft::Xna::Framework::Game* game, AsteroidType asteroidType,
                 Microsoft::Xna::Framework::Vector3 position);
        [[nodiscard]] bool getDestroyedProperty() const;
        void setDestroyedProperty(bool value);
        void Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;

    private:
        float roll_ = 0.0f;
        float pitch_ = 0.0f;
        float yaw_ = 0.0f;
        bool destroyed_ = false;
        float rollIncrement_ = 0.0f;
        float pitchIncrement_ = 0.0f;
        float yawIncrement_ = 0.0f;
        static System::Random random_;
    };
}
