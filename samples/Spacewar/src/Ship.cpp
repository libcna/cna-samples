// SPDX-License-Identifier: MS-PL

#include "Ship.hpp"

#include <array>
#include <cmath>

#include "EvolvedShape.hpp"
#include "Particles.hpp"
#include "Projectiles.hpp"
#include "RetroShip.hpp"
#include "Sound.hpp"
#include "SpacewarGame.hpp"
#include "XInputHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;

    namespace
    {
        constexpr float NinetyDegrees = 3.14159265358979323846f / 2.0f;
        constexpr float ShotSpeed = 150.0f;
        constexpr double HyperspaceTime = 1.5;
        const std::array<std::array<Vector3, 3>, 2> BulletOffsets{{
            {{Vector3(0, 0, -2134.0f), Vector3(0, 0, -782.0f), Vector3(0, 0, -1068.0f)}},
            {{Vector3(0, 0, -2134.0f), Vector3(0, 0, -859.0f), Vector3(0, 0, -866.0f)}},
        }};
    }

    System::Random Ship::random_;

    Ship::Ship(Game* game, PlayerIndex player, ShipClass shipNumber, int shipSkin,
               Vector3 initialPosition, Projectiles* bullets, Particles* particles)
        : SpacewarSceneItem(game,
              std::make_unique<EvolvedShape>(game, EvolvedShapes::Ship, player,
                                             static_cast<int>(shipNumber), shipSkin, LightingType::InGame),
              initialPosition),
          bullets_(bullets), particles_(particles), evolved_(true), player_(player)
    {
        scale_ = Vector3(SpacewarGame::getSettingsProperty().ShipScale);
        rotation_ = Vector3(MathHelper::ToRadians(90.0f), 0.0f, 0.0f);
        direction_ = Vector3(-std::sin(rotation_.Z), std::cos(rotation_.Z), 0.0f);
        if (game)
            batch_ = std::make_unique<SpriteBatch>(game->getGraphicsDeviceProperty());
        if (shipNumber == ShipClass::Pencil)
            extendedExtent_ = std::make_unique<std::array<Vector3, 2>>();
    }

    Ship::Ship(Game* game, PlayerIndex player, Vector3 initialPosition, Projectiles* bullets)
        : SpacewarSceneItem(game, std::make_unique<RetroShip>(game), initialPosition),
          bullets_(bullets), evolved_(false), player_(player)
    {
        scale_ = Vector3(8.0f, 8.0f, 8.0f);
    }

    bool Ship::getInvulnerableProperty() const { return invulnerable_; }
    void Ship::setInvulnerableProperty(bool value) { invulnerable_ = value; }
    bool Ship::getUncollidableProperty() const { return inHyperspace_ || inRecovery_; }
    std::array<Vector3, 2>* Ship::getExtendedExtentProperty() { return extendedExtent_.get(); }
    const std::array<Vector3, 2>* Ship::getExtendedExtentProperty() const { return extendedExtent_.get(); }
    PlayerIndex Ship::getPlayerProperty() const { return player_; }

    void Ship::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        acceleration_ = Vector3::Zero;
        auto& pad = XInputHelper::getGamePadsProperty()[player_];

        if (!getPausedProperty())
        {
            if (!inHyperspace_ && !inRecovery_)
            {
                if (pad.getLeftTriggerPressedProperty() || pad.getBPressedProperty())
                {
                    inHyperspace_ = true;
                    playedReturn_ = false;
                    exitHyperspaceTime_ = time.getTotalSecondsProperty() + HyperspaceTime;
                    Sound::PlayCue(Sounds::HyperspaceActivate);
                }

                if (pad.getRightTriggerPressedProperty() || pad.getAPressedProperty())
                {
                    direction_ = Vector3(-std::sin(rotation_.Z), std::cos(rotation_.Z), 0.0f);
                    direction_.Normalize();
                    if (evolved_)
                    {
                        const auto playerIndex = static_cast<std::size_t>(player_);
                        const auto shipClass = SpacewarGame::getPlayersProperty()[playerIndex].getShipClassProperty();
                        const Vector3 muzzle = Vector3::Transform(
                            BulletOffsets[playerIndex][static_cast<std::size_t>(shipClass)], shape_->getWorldProperty());
                        const Vector3 shotVelocity = SpacewarGame::getPlayersProperty()[playerIndex].getProjectileTypeProperty() == ProjectileType::Rocket
                            ? Vector3::Multiply(direction_, ShotSpeed)
                            : velocity_ + Vector3::Multiply(direction_, ShotSpeed);
                        bullets_->Add(player_, muzzle, shotVelocity, rotation_.Z, time, particles_);
                    }
                    else
                    {
                        bullets_->Add(player_, Vector3::Transform(Vector3(0, 1.1f, 0), shape_->getWorldProperty()),
                                      velocity_ + Vector3::Multiply(direction_, ShotSpeed), rotation_.Z, time, nullptr);
                    }
                }
            }

            if (inHyperspace_)
            {
                if (time.getTotalSecondsProperty() > exitHyperspaceTime_ - 1.3 && !playedReturn_)
                {
                    playedReturn_ = true;
                    Sound::PlayCue(Sounds::HyperspaceReturn);
                }
                if (time.getTotalSecondsProperty() > exitHyperspaceTime_)
                {
                    inHyperspace_ = false;
                    invulnerable_ = true;
                    position_ = Vector3(static_cast<float>(random_.NextDouble() * 800.0 - 400.0),
                                        static_cast<float>(random_.NextDouble() * 500.0 - 250.0), 0.0f);
                }
            }
            else if (inRecovery_)
            {
                if (time.getTotalSecondsProperty() > exitRecoveryTime_)
                {
                    inRecovery_ = false;
                    invulnerable_ = true;
                }
            }
            else
            {
                rotation_.Z -= static_cast<float>(pad.getThumbStickLeftXProperty() *
                    elapsedTime.getTotalSecondsProperty() * 3.0);
                if (pad.getThumbStickLeftYProperty() != 0.0f)
                {
                    if (!playingThrustSound_)
                    {
                        GamePad::SetVibration(player_, 0.8f, 0.2f);
                        cue_ = Sound::Play(player_ == PlayerIndex::One ? Sounds::ThrustPlayer1 : Sounds::ThrustPlayer2);
                        playingThrustSound_ = true;
                    }
                    showThrust_ = true;
                    thrustFrame_ += elapsedTime.getTotalSecondsProperty() * 12.0;
                    if (thrustFrame_ > 12.0) thrustFrame_ = 12.0;
                    const float factor = pad.getThumbStickLeftYProperty();
                    const Vector2 thrustDirection(
                        -SpacewarGame::getSettingsProperty().ThrustPower * factor * std::sin(rotation_.Z),
                         SpacewarGame::getSettingsProperty().ThrustPower * factor * std::cos(rotation_.Z));
                    acceleration_ += Vector3(thrustDirection.X, thrustDirection.Y, 0.0f);
                }
                else
                {
                    thrustFrame_ += elapsedTime.getTotalSecondsProperty() * 12.0;
                    if (thrustFrame_ > 29.0)
                    {
                        showThrust_ = false;
                        thrustFrame_ = 0.0;
                    }
                    if (playingThrustSound_)
                    {
                        GamePad::SetVibration(player_, 0.0f, 0.0f);
                        Sound::Stop(cue_.get());
                        cue_.reset();
                        playingThrustSound_ = false;
                    }
                }
                if (evolved_)
                    acceleration_ -= velocity_ * SpacewarGame::getSettingsProperty().FrictionFactor;
            }
        }

        SpacewarSceneItem::Update(time, elapsedTime);
        if (velocity_.Length() > SpacewarGame::getSettingsProperty().MaxSpeed)
            velocity_ = Vector3::Normalize(velocity_) * SpacewarGame::getSettingsProperty().MaxSpeed;
    }

    void Ship::ResetShip(System::TimeSpan gameTime, Vector3 newPosition)
    {
        position_ = newPosition;
        rotation_.Z = 0.0f;
        velocity_ = Vector3::Zero;
        inRecovery_ = true;
        invulnerable_ = true;
        inHyperspace_ = false;
        exitRecoveryTime_ = gameTime.getTotalSecondsProperty() + SpacewarGame::getSettingsProperty().ShipRecoveryTime;
    }

    void Ship::Silence()
    {
        if (playingThrustSound_)
        {
            Sound::Stop(cue_.get());
            cue_.reset();
            playingThrustSound_ = false;
            GamePad::SetVibration(player_, 0.0f, 0.0f);
        }
    }

    void Ship::OnCreateDevice()
    {
        batch_ = std::make_unique<SpriteBatch>(getGameInstanceProperty()->getGraphicsDeviceProperty());
    }

    const std::vector<Vector4>& Ship::EngineOffsets(PlayerIndex player, ShipClass shipClass)
    {
        static const std::array<std::array<std::vector<Vector4>, 3>, 2> values{{
            {{
                {Vector4(0, 640, 1146, 1), Vector4(-526, -262, 1146, 1), Vector4(526, -262, 1146, 1)},
                {Vector4(-1125, 135, 432, 1), Vector4(1125, 135, 432, 1), Vector4(0, 0, 1020, 1)},
                {Vector4(0, 500, 1140, 1)},
            }},
            {{
                {Vector4(-390, 100, 2013, 1), Vector4(390, 100, 2013, 1), Vector4(0, -240, 1915, 1)},
                {Vector4(0, 270, 1013, 1), Vector4(-380, 635, 628, 1), Vector4(380, 635, 628, 1)},
                {Vector4(-270, 150, 656, 1), Vector4(270, 150, 656, 1), Vector4(0, 0, 740, 1)},
            }},
        }};
        return values[static_cast<std::size_t>(player)][static_cast<std::size_t>(shipClass)];
    }

    void Ship::Render()
    {
        if (inHyperspace_ || inRecovery_)
            return;
        SpacewarSceneItem::Render();
        if (showThrust_ && evolved_)
        {
            auto engine = SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                SpacewarGame::getSettingsProperty().MediaPath + "textures/thrust_stripSmall");
            batch_->Begin(SpriteSortMode::Deferred, BlendState::AlphaBlend);
            const ShipClass shipClass = SpacewarGame::getPlayersProperty()[static_cast<std::size_t>(player_)].getShipClassProperty();
            for (const Vector4& engineOffset : EngineOffsets(player_, shipClass))
            {
                const Vector4 source = Vector4::Transform(
                    engineOffset, shape_->getWorldProperty() * SpacewarGame::getCameraProperty().getViewProperty() *
                    SpacewarGame::getCameraProperty().getProjectionProperty());
                const Vector2 source2D(
                    static_cast<float>(static_cast<int>((source.X / source.W + 1.0f) / 2.0f * 1280.0f)),
                    static_cast<float>(static_cast<int>((-source.Y / source.W + 1.0f) / 2.0f * 720.0f)));
                batch_->Draw(engine, source2D,
                    Rectangle(static_cast<int>(thrustFrame_) * 64, 0, 64, 16), Color::White,
                    -rotation_.Z + NinetyDegrees, Vector2(2.0f, 8.0f), 1.0f,
                    SpriteEffects::None, 0.1f);
            }
            batch_->End();
        }
    }
}
