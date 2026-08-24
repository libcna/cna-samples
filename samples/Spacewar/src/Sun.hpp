// SPDX-License-Identifier: MS-PL
#pragma once

#include "SceneItem.hpp"

namespace Spacewar
{
    class Sun final : public SceneItem
    {
    public:
        Sun(Microsoft::Xna::Framework::Game* game, std::unique_ptr<Shape> shape,
            Microsoft::Xna::Framework::Vector3 position);
        void Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
    };
}
