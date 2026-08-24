// SPDX-License-Identifier: MS-PL

#include "SpacewarSceneItem.hpp"

#include <algorithm>
#include <cmath>

#include "Shape.hpp"
#include "SpacewarGame.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;

    SpacewarSceneItem::SpacewarSceneItem(Game* game)
        : SceneItem(game)
    {
    }

    SpacewarSceneItem::SpacewarSceneItem(Game* game, std::unique_ptr<Shape> shape, Vector3 initialPosition)
        : SceneItem(game, std::move(shape), initialPosition)
    {
    }

    void SpacewarSceneItem::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        if (!getPausedProperty())
        {
            const Vector3 forceDirection = position_ - Vector3(SpacewarGame::getSettingsProperty().SunPosition, 0.0f);
            const double distancePower = std::pow(forceDirection.Length(), SpacewarGame::getSettingsProperty().GravityPower);
            const double factor = std::min(SpacewarGame::getSettingsProperty().GravityStrength / distancePower, 100.0);
            const Vector3 gravityAcceleration = Vector3::Multiply(Vector3::Normalize(forceDirection), static_cast<float>(factor));
            acceleration_ -= gravityAcceleration;
        }

        SceneItem::Update(time, elapsedTime);
        acceleration_ = Vector3::Zero;

        if (SpacewarGame::getGameStateProperty() == GameState::PlayEvolved)
        {
            if (position_.X > 400.0f) position_.X = -400.0f;
            else if (position_.X < -400.0f) position_.X = 400.0f;
        }
        else
        {
            if (position_.X > 300.0f) position_.X = -300.0f;
            else if (position_.X < -300.0f) position_.X = 300.0f;
        }

        if (position_.Y > 250.0f) position_.Y = -250.0f;
        else if (position_.Y < -250.0f) position_.Y = 250.0f;
    }
}
