// SPDX-License-Identifier: MS-PL

#include "Screens/LoadingScreen.hpp"

#include <utility>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace GameStateManagement
{
    using namespace Microsoft::Xna::Framework;

    LoadingScreen::LoadingScreen(ScreenManager&, bool loadingIsSlow,
                                 std::vector<std::shared_ptr<GameScreen>> screensToLoad)
        : loadingIsSlow_(loadingIsSlow), screensToLoad_(std::move(screensToLoad))
    {
        setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(0.5));
    }

    void LoadingScreen::Load(ScreenManager& screenManager, bool loadingIsSlow,
        std::optional<PlayerIndex> controllingPlayer,
        std::vector<std::shared_ptr<GameScreen>> screensToLoad)
    {
        for (const auto& screen : screenManager.GetScreens())
            screen->ExitScreen();
        std::shared_ptr<LoadingScreen> loadingScreen(
            new LoadingScreen(screenManager, loadingIsSlow, std::move(screensToLoad)));
        screenManager.AddScreen(std::move(loadingScreen), controllingPlayer);
    }

    void LoadingScreen::Update(GameTime& gameTime, bool otherScreenHasFocus,
                               bool coveredByOtherScreen)
    {
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
        if (otherScreensAreGone_)
        {
            auto& manager = getScreenManagerProperty();
            manager.RemoveScreen(this);
            for (const auto& screen : screensToLoad_)
                if (screen)
                    manager.AddScreen(screen, getControllingPlayerProperty());
            manager.getGameProperty().ResetElapsedTime();
        }
    }

    void LoadingScreen::Draw(const GameTime&)
    {
        auto& manager = getScreenManagerProperty();
        if (getScreenStateProperty() == ScreenState::Active && manager.GetScreens().size() == 1)
            otherScreensAreGone_ = true;
        if (!loadingIsSlow_)
            return;

        auto& font = manager.getFontProperty();
        const std::string message = "Loading...";
        const auto viewport = manager.getGraphicsDeviceProperty().getViewportProperty();
        const Vector2 viewportSize(static_cast<float>(viewport.getWidthProperty()),
                                   static_cast<float>(viewport.getHeightProperty()));
        const Vector2 textSize = font.MeasureString(message);
        const Vector2 textPosition = (viewportSize - textSize) / 2.0f;
        const Color color = Color::White * getTransitionAlphaProperty();
        auto& spriteBatch = manager.getSpriteBatchProperty();
        spriteBatch.Begin();
        spriteBatch.DrawString(font, message, textPosition, color);
        spriteBatch.End();
    }

    const std::string& LoadingScreen::GetTypeName() const
    {
        static const std::string name = "GameStateManagement.LoadingScreen";
        return name;
    }
}
