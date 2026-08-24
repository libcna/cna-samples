// SPDX-License-Identifier: MS-PL
#pragma once

#include "SceneItem.hpp"

namespace Spacewar
{
    class SpacewarSceneItem : public SceneItem
    {
    public:
        explicit SpacewarSceneItem(Microsoft::Xna::Framework::Game* game);
        SpacewarSceneItem(Microsoft::Xna::Framework::Game* game, std::unique_ptr<Shape> shape,
                          Microsoft::Xna::Framework::Vector3 initialPosition);
        void Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
    };
}
