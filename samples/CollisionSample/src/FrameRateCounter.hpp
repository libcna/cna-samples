#pragma once
#include <memory>
#include <optional>
#include <string>
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/TimeSpan.hpp"
#include "System/Int32.hpp"

namespace CollisionSample {

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Content;
using namespace Microsoft::Xna::Framework::Graphics;

class FrameRateCounter : public DrawableGameComponent {
    ContentManager content_;
    std::unique_ptr<SpriteBatch> spriteBatch_;
    std::optional<SpriteFont> spriteFont_;

    int frameRate_    = 0;
    int frameCounter_ = 0;
    System::TimeSpan elapsedTime_ = System::TimeSpan::Zero;

public:
    explicit FrameRateCounter(Game& game)
        : DrawableGameComponent(game), content_(&game.getServicesProperty()) {}

    const std::string& GetTypeName() const override {
        static const std::string name = "CollisionSample.FrameRateCounter";
        return name;
    }

protected:
    void LoadContent() override {
        spriteBatch_ = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        spriteFont_.emplace(content_.Load<SpriteFont>("content\\Font"));
    }

    void UnloadContent() override {
        content_.Unload();
    }

public:
    void Update(GameTime& gameTime) override {
        elapsedTime_ = elapsedTime_ + gameTime.getElapsedGameTimeProperty();

        if (elapsedTime_ > System::TimeSpan::FromSeconds(1)) {
            elapsedTime_ = elapsedTime_ - System::TimeSpan::FromSeconds(1);
            frameRate_    = frameCounter_;
            frameCounter_ = 0;
        }
    }

    void Draw(const GameTime&) override {
        frameCounter_++;

        const std::string fps = "fps: " + System::Int32::ToString(frameRate_);

        spriteBatch_->Begin();
        spriteBatch_->DrawString(*spriteFont_, fps, Vector2(32.0f, 32.0f), Color::White);
        spriteBatch_->End();
    }
};

} // namespace CollisionSample
