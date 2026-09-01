// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace UserInterfaceSample {

class LoadingScreen final : public GameScreen {
public:
    static void Load(ScreenManager& screenManager, bool loadingIsSlow,
                     std::optional<PlayerIndex> controllingPlayer,
                     std::vector<std::shared_ptr<GameScreen>> screensToLoad) {
        for (const auto& screen : screenManager.GetScreens())
            screen->ExitScreen();

        std::shared_ptr<LoadingScreen> loadingScreen(
            new LoadingScreen(screenManager, loadingIsSlow, std::move(screensToLoad)));
        screenManager.AddScreen(std::move(loadingScreen), controllingPlayer);
    }

    void Update(GameTime& gameTime, bool otherScreenHasFocus,
                bool coveredByOtherScreen) override {
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

        if (otherScreensAreGone_) {
            GetScreenManager()->RemoveScreen(this);
            for (const auto& screen : screensToLoad_) {
                if (screen)
                    GetScreenManager()->AddScreen(screen, ControllingPlayer());
            }
            GetScreenManager()->getGameProperty().ResetElapsedTime();
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        if (GetScreenState() == ScreenState::Active &&
            GetScreenManager()->GetScreens().size() == 1) {
            otherScreensAreGone_ = true;
        }

        if (loadingIsSlow_) {
            auto& spriteBatch = GetScreenManager()->getSpriteBatchProperty();
            auto& font = GetScreenManager()->getFontProperty();
            const std::string message = "Loading...";
            const auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
            const Microsoft::Xna::Framework::Vector2 viewportSize(
                static_cast<float>(viewport.getWidthProperty()),
                static_cast<float>(viewport.getHeightProperty()));
            const Microsoft::Xna::Framework::Vector2 textSize = font.MeasureString(message);
            const Microsoft::Xna::Framework::Vector2 textPosition =
                (viewportSize - textSize) / 2.0f;
            const Microsoft::Xna::Framework::Color color =
                Microsoft::Xna::Framework::Color::White * TransitionAlpha();

            spriteBatch.Begin();
            spriteBatch.DrawString(font, message, textPosition, color);
            spriteBatch.End();
        }
    }

    CNAEXT [[nodiscard]] static const std::string& StaticAssemblyQualifiedName() {
        static const std::string name =
            "UserInterfaceSample.LoadingScreen, AchievementUISample, Version=1.0.0.0, Culture=neutral, PublicKeyToken=null";
        return name;
    }

    CNAEXT [[nodiscard]] const std::string& GetAssemblyQualifiedName() const override {
        return StaticAssemblyQualifiedName();
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "UserInterfaceSample.LoadingScreen";
        return name;
    }

private:
    LoadingScreen(ScreenManager& screenManager, bool loadingIsSlow,
                  std::vector<std::shared_ptr<GameScreen>> screensToLoad)
        : loadingIsSlow_(loadingIsSlow), screensToLoad_(std::move(screensToLoad)) {
        (void)screenManager;
        setIsSerializable(false);
        setTransitionOnTime(System::TimeSpan::FromSeconds(0.5));
    }

    bool loadingIsSlow_;
    bool otherScreensAreGone_ = false;
    std::vector<std::shared_ptr<GameScreen>> screensToLoad_;
};

} // namespace UserInterfaceSample
