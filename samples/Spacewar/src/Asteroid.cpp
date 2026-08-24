// SPDX-License-Identifier: MS-PL

#include "Asteroid.hpp"

#include "BasicEffectShape.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;

    System::Random Asteroid::random_;

    Asteroid::Asteroid(Game* game, AsteroidType asteroidType, Vector3 position)
        : SpacewarSceneItem(game,
              std::make_unique<BasicEffectShape>(game, BasicEffectShapes::Asteroid,
                                                 static_cast<int>(asteroidType), LightingType::InGame),
              position),
          rollIncrement_(static_cast<float>(random_.NextDouble()) - 0.5f),
          pitchIncrement_(static_cast<float>(random_.NextDouble()) - 0.5f),
          yawIncrement_(static_cast<float>(random_.NextDouble()) - 0.5f)
    {
        if (asteroidType == AsteroidType::Large) radius_ = 15.0f;
        if (asteroidType == AsteroidType::Small) radius_ = 6.0f;
    }

    bool Asteroid::getDestroyedProperty() const { return destroyed_; }
    void Asteroid::setDestroyedProperty(bool value) { destroyed_ = value; }

    void Asteroid::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        const float elapsed = static_cast<float>(elapsedTime.getTotalSecondsProperty());
        roll_ += rollIncrement_ * elapsed;
        yaw_ += yawIncrement_ * elapsed;
        pitch_ += pitchIncrement_ * elapsed;
        rotation_ = Vector3(roll_, pitch_, yaw_);
        SpacewarSceneItem::Update(time, elapsedTime);
    }
}
