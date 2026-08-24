// SPDX-License-Identifier: MS-PL

#include "Projectiles.hpp"

#include "SpacewarGame.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;

    Projectiles::Projectiles(Game* game)
        : SceneItem(game)
    {
        Projectile::getProjectileCountProperty().fill(0);
    }

    void Projectiles::Add(PlayerIndex player, Vector3 position, Vector3 velocity, float angle,
                          System::TimeSpan time, Particles* particles)
    {
        const ProjectileType projectileType = SpacewarGame::getPlayersProperty()[
            static_cast<std::size_t>(player)].getProjectileTypeProperty();
        Vector3 offset = Vector3::Zero;
        if (projectileType == ProjectileType::DoubleMachineGun)
        {
            offset.X = -velocity.Y;
            offset.Y = velocity.X;
            offset.Normalize();
            offset = offset * 10.0f;
        }

        const auto& weapon = SpacewarGame::getSettingsProperty().Weapons[static_cast<std::size_t>(projectileType)];
        for (int i = 0; i < weapon.Burst; ++i)
        {
            if (Projectile::getProjectileCountProperty()[static_cast<std::size_t>(player)] < weapon.Max)
            {
                SceneItem::Add(std::make_unique<Projectile>(getGameInstanceProperty(), player,
                    position + velocity * static_cast<float>(i) * 0.1f + offset,
                    velocity, angle, time, particles));
                if (offset != Vector3::Zero)
                {
                    SceneItem::Add(std::make_unique<Projectile>(getGameInstanceProperty(), player,
                        position + velocity * static_cast<float>(i) * 0.1f - offset,
                        velocity, angle, time, particles));
                }
            }
        }
    }

    void Projectiles::Clear()
    {
        for (const auto& item : getChildrenProperty())
            static_cast<Projectile*>(item.get())->DeleteProjectile();
    }
}
