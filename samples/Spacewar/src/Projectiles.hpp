// SPDX-License-Identifier: MS-PL
#pragma once

#include "Projectile.hpp"

namespace Spacewar
{
    class Projectiles : public SceneItem
    {
    public:
        explicit Projectiles(Microsoft::Xna::Framework::Game* game);
        virtual void Add(Microsoft::Xna::Framework::PlayerIndex player,
                         Microsoft::Xna::Framework::Vector3 position,
                         Microsoft::Xna::Framework::Vector3 velocity,
                         float angle, System::TimeSpan time, Particles* particles);
        void Clear() override;
    };
}
