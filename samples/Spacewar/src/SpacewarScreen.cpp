// SPDX-License-Identifier: MS-PL

#include "SpacewarScreen.hpp"

#include "Particles.hpp"
#include "Projectile.hpp"
#include "Projectiles.hpp"
#include "Shape.hpp"
#include "Ship.hpp"
#include "Sound.hpp"
#include "SpacewarGame.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;

    SpacewarScreen::SpacewarScreen(Game* game) : Screen(game) {}

    void SpacewarScreen::HandleCollisions(System::TimeSpan gameTime)
    {
        if (paused_) return;
        if (sun_->Collide(ship1_) && !ship1_->getUncollidableProperty()) HitPlayer1(gameTime, 5);
        if (sun_->Collide(ship2_) && !ship2_->getUncollidableProperty()) HitPlayer2(gameTime, 5);

        if (ship1_->Collide(ship2_) && !ship1_->getUncollidableProperty() && !ship2_->getUncollidableProperty())
        {
            if (SpacewarGame::getGameStateProperty() == GameState::PlayEvolved)
            {
                HitPlayer1(gameTime, 2);
                HitPlayer2(gameTime, 2);
                const float ship1Speed = ship1_->getVelocityProperty().Length();
                const float ship2Speed = ship2_->getVelocityProperty().Length();
                Vector3 tmp = ship1_->getVelocityProperty();
                Vector3 vel = ship2_->getVelocityProperty();
                if (vel.LengthSquared() > 0.0f) vel.Normalize();
                ship1_->setVelocityProperty(vel * ship1Speed);
                if (tmp.LengthSquared() > 0.0f) tmp.Normalize();
                ship2_->setVelocityProperty(tmp * ship2Speed);
            }
            else
            {
                HitPlayer1(gameTime, 5);
                HitPlayer2(gameTime, 5);
            }
        }

        for (const auto& item : bullets_->getChildrenProperty())
        {
            auto* bullet = static_cast<Projectile*>(item.get());
            if (bullet->Collide(sun_)) bullet->DeleteProjectile();
            if (bullet->Collide(ship1_) && !ship1_->getUncollidableProperty())
            {
                bullet->DeleteProjectile();
                HitPlayer1(gameTime, bullet->getDamageProperty());
            }
            if (bullet->Collide(ship2_) && !ship2_->getUncollidableProperty())
            {
                bullet->DeleteProjectile();
                HitPlayer2(gameTime, bullet->getDamageProperty());
            }
        }
    }

    void SpacewarScreen::HitPlayer2(System::TimeSpan gameTime, int damage)
    {
        auto& player = SpacewarGame::getPlayersProperty()[1];
        player.setHealthProperty(player.getHealthProperty() - damage);
        if (player.getHealthProperty() <= 0)
        {
            player.setHealthProperty(5);
            if (SpacewarGame::getGameStateProperty() == GameState::PlayEvolved)
                particles_->AddExplosion(ship2_->getPositionProperty());
            ++player1Score_;
            Sound::PlayCue(Sounds::ExplodeShip);
            ResetShips(gameTime);
        }
        else Sound::PlayCue(Sounds::DamageShip);
    }

    void SpacewarScreen::HitPlayer1(System::TimeSpan gameTime, int damage)
    {
        auto& player = SpacewarGame::getPlayersProperty()[0];
        player.setHealthProperty(player.getHealthProperty() - damage);
        if (player.getHealthProperty() <= 0)
        {
            player.setHealthProperty(5);
            if (SpacewarGame::getGameStateProperty() == GameState::PlayEvolved)
                particles_->AddExplosion(ship1_->getPositionProperty());
            ++player2Score_;
            Sound::PlayCue(Sounds::ExplodeShip);
            ResetShips(gameTime);
        }
        else Sound::PlayCue(Sounds::DamageShip);
    }

    void SpacewarScreen::ResetShips(System::TimeSpan gameTime)
    {
        if (SpacewarGame::getGameStateProperty() == GameState::PlayEvolved)
        {
            ship2_->ResetShip(gameTime, Vector3(SpacewarGame::getSettingsProperty().Ships[1].StartPosition, 0.0f));
            ship1_->ResetShip(gameTime, Vector3(SpacewarGame::getSettingsProperty().Ships[0].StartPosition, 0.0f));
        }
        else
        {
            ship2_->ResetShip(gameTime, Vector3(250, 0, 0));
            ship1_->ResetShip(gameTime, Vector3(-250, 0, 0));
        }
        ship1_->Silence();
        ship2_->Silence();
        bullets_->Clear();
    }

    void SpacewarScreen::Shutdown()
    {
        ship1_->Silence();
        ship2_->Silence();
        Screen::Shutdown();
    }

    void SpacewarScreen::OnCreateDevice()
    {
        Screen::OnCreateDevice();
        ship1_->OnCreateDevice();
        ship2_->OnCreateDevice();
        ship1_->getShapeItemProperty()->OnCreateDevice();
        ship2_->getShapeItemProperty()->OnCreateDevice();
        sun_->getShapeItemProperty()->OnCreateDevice();
        backdrop_->getShapeItemProperty()->OnCreateDevice();
    }
}
