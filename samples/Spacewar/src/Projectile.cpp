// SPDX-License-Identifier: MS-PL

#include "Projectile.hpp"

#include "BasicEffectShape.hpp"
#include "Particles.hpp"
#include "Ship.hpp"
#include "Sound.hpp"
#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;

    std::array<int, 3> Projectile::projectileCount_{};

    Projectile::Projectile(Game* game, PlayerIndex player, Vector3 position, Vector3 velocity,
                           float angle, System::TimeSpan time, Particles* particles)
        : SpacewarSceneItem(game), player_(player)
    {
        velocity_ = velocity;
        position_ = position;
        ++projectileCount_[static_cast<std::size_t>(player_)];

        if (SpacewarGame::getGameStateProperty() == GameState::PlayEvolved)
        {
            projectileType_ = static_cast<int>(
                SpacewarGame::getPlayersProperty()[static_cast<std::size_t>(player_)].getProjectileTypeProperty());
            particles_ = particles;
            const auto& weapon = SpacewarGame::getSettingsProperty().Weapons[static_cast<std::size_t>(projectileType_)];
            endTime_ = time.getTotalSecondsProperty() + weapon.Lifetime;
            thrust_ = Vector3::Multiply(Vector3::Normalize(velocity), weapon.Acceleration);
            radius_ = 2.0f;
            damage_ = weapon.Damage;
            shape_ = std::make_unique<BasicEffectShape>(game, BasicEffectShapes::Projectile,
                                                        projectileType_, LightingType::InGame);
            scale_.X = scale_.Y = scale_.Z = SpacewarGame::getSettingsProperty().BulletScale;
            rotation_.X = MathHelper::ToRadians(90.0f);
            rotation_.Y = 0.0f;
            rotation_.Z = angle;
        }
        else
        {
            damage_ = 5;
            projectileType_ = static_cast<int>(ProjectileType::Peashooter);
            radius_ = 1.0f;
            endTime_ = time.getTotalSecondsProperty() + 2.0;
            acceleration_ = Vector3::Zero;
        }

        switch (static_cast<ProjectileType>(projectileType_))
        {
            case ProjectileType::Peashooter: Sound::PlayCue(Sounds::PeashooterFire); break;
            case ProjectileType::MachineGun: Sound::PlayCue(Sounds::MachineGunFire); break;
            case ProjectileType::DoubleMachineGun: Sound::PlayCue(Sounds::DoubleMachineGunFire); break;
            case ProjectileType::Rocket: Sound::PlayCue(Sounds::RocketExplode); break;
            case ProjectileType::BFG: Sound::PlayCue(Sounds::BFGFire); break;
        }
    }

    std::array<int, 3>& Projectile::getProjectileCountProperty() { return projectileCount_; }
    int Projectile::getDamageProperty() const { return damage_; }

    void Projectile::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        if (time.getTotalSecondsProperty() > endTime_)
        {
            if (SpacewarGame::getGameStateProperty() == GameState::PlayEvolved && projectileType_ == 4 && !exploded_)
            {
                Sound::PlayCue(Sounds::Explosion);
                particles_->AddExplosion(getPositionProperty());
                exploded_ = true;
                radius_ = 30.0f;
                damage_ = 3;
            }
            else
                DeleteProjectile();
        }

        acceleration_ = thrust_;
        if (projectileType_ == 3)
            particles_->AddRocketTrail(shape_->getWorldProperty(), Vector2(acceleration_.X, -acceleration_.Y));
        SpacewarSceneItem::Update(time, elapsedTime);
    }

    void Projectile::DeleteProjectile()
    {
        if (!delete_)
            --projectileCount_[static_cast<std::size_t>(player_)];
        delete_ = true;
    }

    bool Projectile::Collide(SceneItem* item)
    {
        const float currentDistance = (getPositionProperty() - item->getPositionProperty()).Length();
        bool colliding = SpacewarSceneItem::Collide(item);
        if (const auto* shipItem = dynamic_cast<const Ship*>(item); shipItem && shipItem->getPlayerProperty() == player_)
        {
            if (colliding && !projectileArmed_)
                colliding = false;
            else if (!colliding && !projectileArmed_ && currentDistance > item->getRadiusProperty() * 2.0f)
                projectileArmed_ = true;
        }
        return colliding;
    }
}
