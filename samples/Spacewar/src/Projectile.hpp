// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>

#include "SpacewarSceneItem.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"

namespace Spacewar
{
    class Particles;

    class Projectile : public SpacewarSceneItem
    {
    public:
        Projectile(Microsoft::Xna::Framework::Game* game,
                   Microsoft::Xna::Framework::PlayerIndex player,
                   Microsoft::Xna::Framework::Vector3 position,
                   Microsoft::Xna::Framework::Vector3 velocity,
                   float angle, System::TimeSpan time, Particles* particles);

        [[nodiscard]] static std::array<int, 3>& getProjectileCountProperty();
        [[nodiscard]] int getDamageProperty() const;
        void Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
        void DeleteProjectile();
        bool Collide(SceneItem* item) override;

    private:
        static std::array<int, 3> projectileCount_;
        Microsoft::Xna::Framework::PlayerIndex player_;
        int damage_ = 0;
        double endTime_ = 0.0;
        int projectileType_ = 0;
        bool exploded_ = false;
        Particles* particles_ = nullptr;
        bool projectileArmed_ = false;
        Microsoft::Xna::Framework::Vector3 thrust_;
    };
}
