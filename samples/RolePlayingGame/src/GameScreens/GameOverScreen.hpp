#pragma once

// GameOverScreen.hpp -- C++ port of GameScreens/GameOverScreen.cs.

#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../AudioManager.hpp"
#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Displays the game-over screen, after the player has lost.
class GameOverScreen : public GameScreen {
public:
    // Create a new GameOverScreen object.
    GameOverScreen() {
        AudioManager::PushMusic("LoseTheme");
        Exiting += [](System::Object*, const System::EventArgs&) { AudioManager::PopMusic(); };
    }

    void LoadContent() override {
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();
        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();

        fadeTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");
        backTexture_ = content.Load<Texture2D>("Textures/GameScreens/PopupScreen");
        selectIconTexture_ = content.Load<Texture2D>("Textures/Buttons/AButton");

        backgroundPosition_.X =
            (float)((viewport.getWidthProperty() - backTexture_.getWidthProperty()) / 2);
        backgroundPosition_.Y =
            (float)((viewport.getHeightProperty() - backTexture_.getHeightProperty()) / 2);

        titlePosition_.X =
            ((float)viewport.getWidthProperty() - Fonts::HeaderFont().MeasureString(titleString_).X) /
            2.0f;
        titlePosition_.Y = backgroundPosition_.Y + 70.0f;

        gameOverPosition_.X = ((float)viewport.getWidthProperty() -
                               Fonts::ButtonNamesFont().MeasureString(titleString_).X) / 2.0f;
        gameOverPosition_.Y =
            backgroundPosition_.Y + (float)(backTexture_.getHeightProperty() / 2);

        selectIconPosition_.X = (float)(viewport.getWidthProperty() / 2 + 260);
        selectIconPosition_.Y = backgroundPosition_.Y + 530.0f;
        selectPosition_.X = selectIconPosition_.X -
                            Fonts::ButtonNamesFont().MeasureString(selectString_).X - 10.0f;
        selectPosition_.Y = backgroundPosition_.Y + 530.0f;
    }

    // Handles user input.
    // Defined out-of-line in MainMenuScreen.hpp -- it opens the main menu.
    void HandleInput() override;

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        spriteBatch.Begin();

        // Draw fading screen
        spriteBatch.Draw(fadeTexture_, Rectangle(0, 0, 1280, 720), Color::White);

        // Draw popup texture
        spriteBatch.Draw(backTexture_, backgroundPosition_, Color::White);

        // Draw title
        spriteBatch.DrawString(Fonts::HeaderFont(), titleString_, titlePosition_,
                               Fonts::TitleColor);

        // Draw Gameover text
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), gameOverString_, gameOverPosition_,
                               Fonts::CountColor);

        // Draw select button
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), selectString_, selectPosition_,
                               Color::White);
        spriteBatch.Draw(selectIconTexture_, selectIconPosition_, Color::White);

        spriteBatch.End();
    }

private:
    Texture2D backTexture_;
    Texture2D selectIconTexture_;
    Texture2D fadeTexture_;
    Vector2 backgroundPosition_;
    Vector2 titlePosition_;
    Vector2 gameOverPosition_;
    Vector2 selectPosition_;
    Vector2 selectIconPosition_;

    const std::string titleString_ = "Game Over";
    const std::string gameOverString_ = "The party has been defeated.";
    const std::string selectString_ = "Continue";
};

} // namespace RolePlaying
