// SPDX-License-Identifier: MS-PL

#include "EvolvedScreen.hpp"

#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>

#include "Asteroid.hpp"
#include "EvolvedBackdrop.hpp"
#include "EvolvedSun.hpp"
#include "Font.hpp"
#include "Particles.hpp"
#include "Projectile.hpp"
#include "Projectiles.hpp"
#include "Ship.hpp"
#include "Sound.hpp"
#include "SpacewarGame.hpp"
#include "Sun.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/String.hpp"
#include <cmath>
#include "System/Int32.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    namespace
    {
        const std::array<std::string, 4> ScoreLookup{"000", "001", "011", "111"};
        const std::array<Vector3, 7> AsteroidStarts{
            Vector3(390, 240, 0), Vector3(-390, -240, 0), Vector3(390, -240, 0),
            Vector3(-390, 240, 0), Vector3(-390, 240, 0), Vector3(390, 240, 0),
            Vector3(-390, -240, 0),
        };
    }

    EvolvedScreen::EvolvedScreen(Game* game)
        : SpacewarScreen(game)
    {
        auto backdrop = std::make_unique<SceneItem>(game, std::make_unique<EvolvedBackdrop>(game));
        backdrop_ = backdrop.get();
        backdrop_->setCenterProperty(Vector3(0.5f, 0.5f, 0.0f));
        backdrop_->setScaleProperty(Vector3(16.0f * 46.0f, 9.0f * 46.0f, 1.0f));
        backdrop_->setPositionProperty(Vector3(-0.5f, -0.5f, 0.0f));
        scene_->Add(std::move(backdrop));

        auto bullets = std::make_unique<Projectiles>(game);
        bullets_ = bullets.get();
        auto particles = std::make_unique<Particles>(game);
        particles_ = particles.get();

        auto ship1 = std::make_unique<Ship>(
            game, PlayerIndex::One, SpacewarGame::getPlayersProperty()[0].getShipClassProperty(),
            SpacewarGame::getPlayersProperty()[0].getSkinProperty(),
            Vector3(SpacewarGame::getSettingsProperty().Ships[0].StartPosition, 0.0f), bullets_, particles_);
        ship1_ = ship1.get();
        ship1_->setPausedProperty(true);
        ship1_->setRadiusProperty(15.0f);
        if (SpacewarGame::getPlayersProperty()[0].getShipClassProperty() == ShipClass::Pencil)
        {
            (*ship1_->getExtendedExtentProperty())[0] = Vector3(0, 25, 0);
            (*ship1_->getExtendedExtentProperty())[1] = Vector3(0, -25, 0);
        }

        auto ship2 = std::make_unique<Ship>(
            game, PlayerIndex::Two, SpacewarGame::getPlayersProperty()[1].getShipClassProperty(),
            SpacewarGame::getPlayersProperty()[1].getSkinProperty(),
            Vector3(SpacewarGame::getSettingsProperty().Ships[1].StartPosition, 0.0f), bullets_, particles_);
        ship2_ = ship2.get();
        ship2_->setPausedProperty(true);
        ship2_->setRadiusProperty(15.0f);
        if (SpacewarGame::getPlayersProperty()[1].getShipClassProperty() == ShipClass::Pencil)
        {
            (*ship2_->getExtendedExtentProperty())[0] = Vector3(0, 25, 0);
            (*ship2_->getExtendedExtentProperty())[1] = Vector3(0, -25, 0);
        }

        scene_->Add(std::move(bullets));
        asteroids_.reserve(static_cast<std::size_t>(SpacewarGame::getGameLevelProperty() + 2));
        for (int i = 0; i < SpacewarGame::getGameLevelProperty() + 2; ++i)
        {
            auto asteroid = std::make_unique<Asteroid>(game,
                random_.NextDouble() > 0.5 ? AsteroidType::Large : AsteroidType::Small,
                AsteroidStarts[static_cast<std::size_t>(i)]);
            Asteroid* raw = asteroid.get();
            raw->setScaleProperty(Vector3(SpacewarGame::getSettingsProperty().AsteroidScale));
            raw->setPausedProperty(true);
            raw->setVelocityProperty(static_cast<float>(random_.Next(100)) * Vector3::Normalize(
                Vector3(static_cast<float>(random_.NextDouble() - 0.5),
                        static_cast<float>(random_.NextDouble() - 0.5), 0.0f)));
            asteroids_.push_back(raw);
            scene_->Add(std::move(asteroid));
        }

        scene_->Add(std::move(ship1));
        scene_->Add(std::move(ship2));
        scene_->Add(std::move(particles));
        sun_ = scene_->Add(std::make_unique<Sun>(game, std::make_unique<EvolvedSun>(game),
                                                 Vector3(-0.5f, -0.5f, 0.0f)));
        SpacewarGame::getPlayersProperty()[0].setHealthProperty(5);
        SpacewarGame::getPlayersProperty()[1].setHealthProperty(5);
    }

    GameState EvolvedScreen::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        if (sunScale_ > 1.0)
        {
            if (sunScale_ > 20.0) sunScale_ = 20.0;
            else sunScale_ -= elapsedTime.getTotalSecondsProperty() * 15.0;
            if (sunScale_ < 1.0)
            {
                sunScale_ = 1.0;
                paused_ = false;
                ship1_->setPausedProperty(false);
                ship2_->setPausedProperty(false);
                for (Asteroid* asteroid : asteroids_) asteroid->setPausedProperty(false);
                levelTime_ = static_cast<float>(SpacewarGame::getSettingsProperty().LevelTime);
            }
            sun_->setScaleProperty(Vector3(
                SpacewarGame::getSettingsProperty().Size * static_cast<float>(sunScale_),
                SpacewarGame::getSettingsProperty().Size * static_cast<float>(sunScale_), 1.0f));
        }

        SpacewarScreen::Update(time, elapsedTime);
        HandleCollisions(time);
        if (!paused_)
        {
            levelTime_ -= static_cast<float>(elapsedTime.getTotalSecondsProperty());
            if (levelTime_ - static_cast<float>(lastLevelTime_) < 0.0f)
            {
                if (levelTime_ < 0.0f) Sound::PlayCue(Sounds::CountDownExpire);
                else if (levelTime_ < 6.0f) Sound::PlayCue(Sounds::CountDownWarning);
            }
            lastLevelTime_ = static_cast<int>(levelTime_);
        }

        if (!paused_ && levelTime_ <= 0.0f)
        {
            paused_ = true;
            ended_ = true;
            ship1_->setPausedProperty(true);
            ship2_->setPausedProperty(true);
            ship1_->Silence();
            ship2_->Silence();
            for (Asteroid* asteroid : asteroids_) asteroid->setPausedProperty(true);
            bullets_->Clear();
            particles_->Clear();
            endTime_ = time.getTotalSecondsProperty() + 5.0;
        }

        if (ended_ && time.getTotalSecondsProperty() > endTime_)
        {
            Shutdown();
            auto& players = SpacewarGame::getPlayersProperty();
            players[0].setCashProperty(players[0].getCashProperty() + player1Score_ * 1000);
            players[1].setCashProperty(players[1].getCashProperty() + player2Score_ * 1000);
            if (player1Score_ > player2Score_)
            {
                players[0].setScoreProperty(players[0].getScoreProperty() + 1);
                SpacewarGame::setGameLevelProperty(SpacewarGame::getGameLevelProperty() + 1);
            }
            else if (player2Score_ > player1Score_)
            {
                players[1].setScoreProperty(players[1].getScoreProperty() + 1);
                SpacewarGame::setGameLevelProperty(SpacewarGame::getGameLevelProperty() + 1);
            }
            if (players[0].getScoreProperty() == 3 || players[1].getScoreProperty() == 3)
                return GameState::Victory;
            return GameState::ShipUpgrade;
        }
        return GameState::None;
    }

    std::string EvolvedScreen::Currency(int value)
    {
        std::string digits = System::Int32::ToString(value);
        for (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(digits.size()) - 3; i > 0; i -= 3)
            digits.insert(static_cast<std::size_t>(i), ",");
        return "$" + digits;
    }

    void EvolvedScreen::Render()
    {
        SpacewarScreen::Render();
        if (ended_)
        {
            auto overlay = SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                SpacewarGame::getSettingsProperty().MediaPath + "textures/In-game_score_overlay");
            batch_->Begin(SpriteSortMode::Deferred, BlendState::Opaque);
            batch_->Draw(overlay, Vector2(70, 200), std::nullopt, Color::White);
            batch_->Draw(overlay, Vector2(900, 200), std::nullopt, Color::White);
            batch_->End();
        }

        Font::Begin();
        if (ended_)
        {
            Font::Draw(FontStyle::WeaponLarge, 100, 240,
                       System::Int32::ToString(player1Score_) + " pts x $1,000");
            Font::Draw(FontStyle::WeaponLarge, 220, 280, "=");
            Font::Draw(FontStyle::WeaponLarge, 180, 320, Currency(player1Score_ * 1000));
            Font::Draw(FontStyle::WeaponLarge, 930, 240,
                       System::Int32::ToString(player2Score_) + " pts x $1,000");
            Font::Draw(FontStyle::WeaponLarge, 1050, 280, "=");
            Font::Draw(FontStyle::WeaponLarge, 1010, 320, Currency(player2Score_ * 1000));
        }
        // String.Format("{0:0}:{1:00}", (int)(levelTime / 60), levelTime % 60). The seconds
        // argument is the FLOAT remainder in the original, not a truncated int: "{1:00}" rounds
        // it, so 59.8 s prints as "60" there and did not here.
        Font::Draw(FontStyle::GameCountDown, 592, 40,
                   System::String::Format("{0:0}:{1:00}",
                                          static_cast<int>(levelTime_ / 60.0f),
                                          static_cast<double>(std::fmod(levelTime_, 60.0f))));
        Font::Draw(FontStyle::GamePlayerNames, 50, 40, "1");
        Font::Draw(FontStyle::GamePlayerNames, 1110, 40, "2");
        const auto& players = SpacewarGame::getPlayersProperty();
        Font::Draw(FontStyle::WeaponIcons, 50, 560, static_cast<int>(players[0].getProjectileTypeProperty()));
        Font::Draw(FontStyle::WeaponIcons, 1090, 560, static_cast<int>(players[1].getProjectileTypeProperty()) + 5);
        Font::Draw(FontStyle::ScoreButtons, 60, 70, ScoreLookup[static_cast<std::size_t>(players[0].getScoreProperty())]);
        Font::Draw(FontStyle::ScoreButtons, 1140, 70, ScoreLookup[static_cast<std::size_t>(players[1].getScoreProperty())]);
        int xOffset = players[0].getProjectileTypeProperty() == ProjectileType::BFG ? 12 : 0;
        Font::Draw(FontStyle::HealthBar, 130 + xOffset, 625, players[0].getHealthProperty());
        xOffset = players[1].getProjectileTypeProperty() == ProjectileType::BFG ? 16 : 0;
        Font::Draw(FontStyle::HealthBar, 1170 + xOffset, 625, players[1].getHealthProperty());
        Font::Draw(FontStyle::Score, 300, 15, player1Score_);
        Font::Draw(FontStyle::Score, 940, 15, player2Score_);
        Font::End();
    }

    void EvolvedScreen::HandleCollisions(System::TimeSpan gameTime)
    {
        SpacewarScreen::HandleCollisions(gameTime);
        bool asteroidHitShip1 = false;
        bool asteroidHitShip2 = false;
        for (Asteroid* asteroid : asteroids_)
        {
            if (asteroid->getDestroyedProperty()) continue;
            if (asteroid->Collide(ship1_) && !ship1_->getUncollidableProperty())
            {
                asteroidHitShip1 = true;
                if (!ship1_->getInvulnerableProperty())
                {
                    const float shipSpeed = ship1_->getVelocityProperty().Length();
                    const float asteroidSpeed = asteroid->getVelocityProperty().Length();
                    Vector3 tmp = ship1_->getVelocityProperty();
                    Vector3 vel = asteroid->getVelocityProperty();
                    vel.Normalize();
                    ship1_->setVelocityProperty(vel * shipSpeed);
                    tmp.Normalize();
                    asteroid->setVelocityProperty(tmp * asteroidSpeed);
                    HitPlayer1(gameTime, 1);
                    ship1_->setInvulnerableProperty(true);
                }
            }
            if (asteroid->Collide(ship2_) && !ship2_->getUncollidableProperty())
            {
                asteroidHitShip2 = true;
                if (!ship2_->getInvulnerableProperty())
                {
                    const float shipSpeed = ship2_->getVelocityProperty().Length();
                    const float asteroidSpeed = asteroid->getVelocityProperty().Length();
                    Vector3 tmp = ship2_->getVelocityProperty();
                    Vector3 vel = asteroid->getVelocityProperty();
                    vel.Normalize();
                    ship2_->setVelocityProperty(vel * shipSpeed);
                    tmp.Normalize();
                    asteroid->setVelocityProperty(tmp * asteroidSpeed);
                    HitPlayer2(gameTime, 1);
                    ship2_->setInvulnerableProperty(true);
                }
            }
            for (const auto& item : bullets_->getChildrenProperty())
            {
                auto* bullet = static_cast<Projectile*>(item.get());
                if (bullet->Collide(asteroid))
                {
                    particles_->AddExplosion(asteroid->getPositionProperty());
                    asteroid->setDeleteProperty(true);
                    asteroid->setDestroyedProperty(true);
                    bullet->DeleteProjectile();
                    Sound::PlayCue(Sounds::Explosion);
                }
            }
        }
        if (!asteroidHitShip1 && ship1_->getInvulnerableProperty()) ship1_->setInvulnerableProperty(false);
        if (!asteroidHitShip2 && ship2_->getInvulnerableProperty()) ship2_->setInvulnerableProperty(false);
    }

    void EvolvedScreen::OnCreateDevice()
    {
        SpacewarScreen::OnCreateDevice();
        for (Asteroid* asteroid : asteroids_) asteroid->getShapeItemProperty()->OnCreateDevice();
        for (const auto& item : bullets_->getChildrenProperty())
            item->getShapeItemProperty()->OnCreateDevice();
        particles_->OnCreateDevice();
    }
}
