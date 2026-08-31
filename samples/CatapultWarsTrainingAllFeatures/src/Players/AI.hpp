// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "System/Random.hpp"

#include "Player.hpp"

namespace CatapultGame {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteEffects;

class AI : public Player {
public:
    explicit AI(Game& game) : Player(game) {}

    AI(Game& game, SpriteBatch& screenSpriteBatch) : Player(game, screenSpriteBatch) {
        setCatapultProperty(std::make_shared<Catapult>(
            game, screenSpriteBatch, "Textures/Catapults/Red/redIdle/redIdle",
            Vector2(600.0f, 332.0f), SpriteEffects::FlipHorizontally, true, false));
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CatapultGame.AI";
        return name;
    }

    void Initialize() override {
        random_ = System::Random();
        catapult_->Initialize();
        Player::Initialize();
    }

    void Update(GameTime& gameTime) override {
        if (catapult_->getCurrentStateProperty() == CatapultState::Aiming &&
            !catapult_->getAnimationRunningProperty()) {
            const float shotVelocity = static_cast<float>(
                random_.Next(static_cast<int>(MinShotVelocity),
                             static_cast<int>(MaxShotVelocity)));
            const float shotAngle = MinShotAngle + static_cast<float>(random_.NextDouble()) *
                (MaxShotAngle - MinShotAngle);
            catapult_->setShotStrengthProperty(shotVelocity / MaxShotVelocity);
            catapult_->setShotVelocityProperty(shotVelocity);
            catapult_->setShotAngleProperty(shotAngle);
        }
        Player::Update(gameTime);
    }

private:
    System::Random random_;
};

} // namespace CatapultGame
