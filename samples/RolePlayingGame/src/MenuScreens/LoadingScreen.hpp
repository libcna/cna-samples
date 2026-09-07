#pragma once

// LoadingScreen.hpp -- C++ port of MenuScreens/LoadingScreen.cs.

#include <cmath>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../Fonts.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// The loading screen coordinates transitions between the menu system and the game itself.
class LoadingScreen : public GameScreen {
public:
    // Activates the loading screen: tells all current screens to transition off, then adds a
    // LoadingScreen that will add screensToLoad once they're gone.
    static void Load(ScreenManager& screenManager, bool loadingIsSlow,
                     std::vector<std::shared_ptr<GameScreen>> screensToLoad) {
        // Tell all the current screens to transition off.
        for (const std::shared_ptr<GameScreen>& screen : screenManager.GetScreens()) {
            screen->ExitScreen();
        }

        // Create and activate the loading screen.
        auto loadingScreen = std::shared_ptr<LoadingScreen>(
            new LoadingScreen(loadingIsSlow, std::move(screensToLoad)));
        screenManager.AddScreen(loadingScreen);
    }

    void LoadContent() override {
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();
        loadingTexture_ = content.Load<Texture2D>("Textures/MainMenu/LoadingPause");
        loadingBlackTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");

        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        loadingBlackTextureDestination_ =
            Rectangle(viewport.getXProperty(), viewport.getYProperty(),
                      viewport.getWidthProperty(), viewport.getHeightProperty());
        loadingPosition_ = Vector2(
            (float)viewport.getXProperty() +
                std::floor((float)(viewport.getWidthProperty() -
                                   loadingTexture_.getWidthProperty()) / 2.0f),
            (float)viewport.getYProperty() +
                std::floor((float)(viewport.getHeightProperty() -
                                   loadingTexture_.getHeightProperty()) / 2.0f));

        GameScreen::LoadContent();
    }

    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override {
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

        // If all the previous screens have finished transitioning off, it is time to actually
        // perform the load.
        if (otherScreensAreGone_) {
            ScreenManager* screenManager = GetScreenManager();
            screenManager->RemoveScreen(this);
            for (const std::shared_ptr<GameScreen>& screen : screensToLoad_) {
                if (screen != nullptr) {
                    screenManager->AddScreen(screen);
                }
            }
            // Once the load has finished, we use ResetElapsedTime to tell the game timing
            // mechanism that we have just finished a very long frame, and that it should not try
            // to catch up.
            screenManager->getGameProperty().ResetElapsedTime();
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        // If we are the only active screen, that means all the previous screens must have
        // finished transitioning off. We check for this in the Draw method, rather than in
        // Update, because it isn't enough just for the screens to be gone: in order for the
        // transition to look good we must have actually drawn a frame without them before we
        // perform the load.
        if (GetScreenState() == ScreenState::Active &&
            GetScreenManager()->GetScreens().size() == 1) {
            otherScreensAreGone_ = true;
        }

        // The gameplay screen takes a while to load, so we display a loading message while that
        // is going on, but the menus load very quickly, and it would look silly if we flashed
        // this up for just a fraction of a second while returning from the game to the menus.
        if (loadingIsSlow_) {
            SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
            spriteBatch.Begin();
            spriteBatch.Draw(loadingBlackTexture_, loadingBlackTextureDestination_, Color::White);
            spriteBatch.Draw(loadingTexture_, loadingPosition_, Color::White);
            spriteBatch.End();
        }
    }

private:
    LoadingScreen(bool loadingIsSlow, std::vector<std::shared_ptr<GameScreen>> screensToLoad)
        : loadingIsSlow_(loadingIsSlow), screensToLoad_(std::move(screensToLoad)) {
        SetTransitionOnTime(System::TimeSpan::FromSeconds(0.5));
    }

    bool loadingIsSlow_;
    bool otherScreensAreGone_ = false;
    std::vector<std::shared_ptr<GameScreen>> screensToLoad_;
    Texture2D loadingTexture_;
    Vector2 loadingPosition_;
    Texture2D loadingBlackTexture_;
    Rectangle loadingBlackTextureDestination_;
};

} // namespace RolePlaying
