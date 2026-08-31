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
            Vector2(1330.0f, 332.0f), SpriteEffects::FlipHorizontally, true));
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
                random_.Next(static_cast<int>(MinShotStrength),
                             static_cast<int>(MaxShotStrength)));
            catapult_->setShotStrengthProperty(shotVelocity / MaxShotStrength);
            catapult_->setShotVelocityProperty(shotVelocity);
        }
        Player::Update(gameTime);
    }

private:
    System::Random random_;
};

} // namespace CatapultGame
