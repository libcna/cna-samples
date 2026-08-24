// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>
#include <vector>

#include "SpacewarSceneItem.hpp"
#include "Enums.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Audio/Cue.hpp"
#include "System/Random.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class SpriteBatch;
}

namespace Spacewar
{
    class Particles;
    class Projectiles;

    class Ship final : public SpacewarSceneItem
    {
    public:
        Ship(Microsoft::Xna::Framework::Game* game, Microsoft::Xna::Framework::PlayerIndex player,
             ShipClass shipNumber, int shipSkin, Microsoft::Xna::Framework::Vector3 initialPosition,
             Projectiles* bullets, Particles* particles);
        Ship(Microsoft::Xna::Framework::Game* game, Microsoft::Xna::Framework::PlayerIndex player,
             Microsoft::Xna::Framework::Vector3 initialPosition, Projectiles* bullets);

        [[nodiscard]] bool getInvulnerableProperty() const;
        void setInvulnerableProperty(bool value);
        [[nodiscard]] bool getUncollidableProperty() const;
        [[nodiscard]] std::array<Microsoft::Xna::Framework::Vector3, 2>* getExtendedExtentProperty();
        [[nodiscard]] const std::array<Microsoft::Xna::Framework::Vector3, 2>* getExtendedExtentProperty() const;
        [[nodiscard]] Microsoft::Xna::Framework::PlayerIndex getPlayerProperty() const;

        void Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;
        void ResetShip(System::TimeSpan gameTime, Microsoft::Xna::Framework::Vector3 newPosition);
        void Silence();
        void OnCreateDevice() override;
        void Render() override;

    private:
        [[nodiscard]] static const std::vector<Microsoft::Xna::Framework::Vector4>&
        EngineOffsets(Microsoft::Xna::Framework::PlayerIndex player, ShipClass shipClass);

        Projectiles* bullets_ = nullptr;
        Particles* particles_ = nullptr;
        double thrustFrame_ = 0.0;
        bool showThrust_ = false;
        bool evolved_ = false;
        Microsoft::Xna::Framework::PlayerIndex player_;
        bool playingThrustSound_ = false;
        std::unique_ptr<Microsoft::Xna::Framework::Audio::Cue> cue_;
        bool inHyperspace_ = false;
        bool inRecovery_ = false;
        double exitHyperspaceTime_ = 0.0;
        double exitRecoveryTime_ = 0.0;
        bool playedReturn_ = false;
        bool invulnerable_ = false;
        static System::Random random_;
        Microsoft::Xna::Framework::Vector3 direction_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> batch_;
        std::unique_ptr<std::array<Microsoft::Xna::Framework::Vector3, 2>> extendedExtent_;
    };
}
