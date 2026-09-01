// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace UserInterfaceSample {

class BackgroundScreen final : public GameScreen {
public:
    BackgroundScreen() {
        setTransitionOnTime(System::TimeSpan::FromSeconds(0.5));
        setTransitionOffTime(System::TimeSpan::FromSeconds(0.5));
    }

    void LoadContent() override {
        if (!content_) {
            content_ = std::make_unique<Microsoft::Xna::Framework::Content::ContentManager>(
                &GetScreenManager()->getGameProperty().getServicesProperty(), "Content");
        }
        backgroundTexture_.emplace(
            content_->Load<Microsoft::Xna::Framework::Graphics::Texture2D>("background"));
    }

    void UnloadContent() override {
        content_->Unload();
        backgroundTexture_.reset();
    }

    void Update(GameTime& gameTime, bool otherScreenHasFocus,
                bool coveredByOtherScreen) override {
        (void)coveredByOtherScreen;
        GameScreen::Update(gameTime, otherScreenHasFocus, false);
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        auto& spriteBatch = GetScreenManager()->getSpriteBatchProperty();
        const auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        const Microsoft::Xna::Framework::Rectangle fullscreen(
            0, 0, viewport.getWidthProperty(), viewport.getHeightProperty());

        spriteBatch.Begin();
        const float alpha = TransitionAlpha();
        spriteBatch.Draw(*backgroundTexture_, fullscreen,
                         Microsoft::Xna::Framework::Color(alpha, alpha, alpha));
        spriteBatch.End();
    }

    CNAEXT [[nodiscard]] static const std::string& StaticAssemblyQualifiedName() {
        static const std::string name =
            "UserInterfaceSample.BackgroundScreen, AchievementUISample, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null";
        return name;
    }

    CNAEXT [[nodiscard]] const std::string& GetAssemblyQualifiedName() const override {
        return StaticAssemblyQualifiedName();
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "UserInterfaceSample.BackgroundScreen";
        return name;
    }

private:
    std::unique_ptr<Microsoft::Xna::Framework::Content::ContentManager> content_;
    std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> backgroundTexture_;
};

} // namespace UserInterfaceSample
