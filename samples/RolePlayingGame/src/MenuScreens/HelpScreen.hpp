#pragma once

// HelpScreen.hpp -- C++ port of MenuScreens/HelpScreen.cs.

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Shows the help screen, explaining the basic game idea to the user.
class HelpScreen : public GameScreen {
public:
    HelpScreen() { textLines_ = Fonts::BreakTextIntoList(helpText_, Fonts::DescriptionFont(), 590); }

    void LoadContent() override {
        GameScreen::LoadContent();

        auto& content = GetScreenManager()->getGameProperty().getContentProperty();

        backgroundTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenu");
        plankTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenuPlank03");
        backTexture_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        scrollUpTexture_ = content.Load<Texture2D>("Textures/GameScreens/ScrollUp");
        scrollDownTexture_ = content.Load<Texture2D>("Textures/GameScreens/ScrollDown");
        lineBorderTexture_ = content.Load<Texture2D>("Textures/GameScreens/LineBorder");

        plankPosition_.X = (float)(backgroundTexture_.getWidthProperty() / 2 -
                                   plankTexture_.getWidthProperty() / 2);
        plankPosition_.Y = 60.0f;

        titlePosition_.X = plankPosition_.X + ((float)plankTexture_.getWidthProperty() -
                                               Fonts::HeaderFont().MeasureString("Help").X) / 2.0f;
        titlePosition_.Y = plankPosition_.Y + ((float)plankTexture_.getHeightProperty() -
                                               Fonts::HeaderFont().MeasureString("Help").Y) / 2.0f;
    }

    void HandleInput() override {
        // exits the screen
        if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            ExitScreen();
            return;
        }
        // scroll down
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
            // Traverse down the help text
            if (startIndex_ + MaxLineDisplay < (int)textLines_.size()) {
                startIndex_ += 1;
            }
        }
        // scroll up
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
            // Traverse up the help text
            if (startIndex_ > 0) {
                startIndex_ -= 1;
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        spriteBatch.Begin();

        spriteBatch.Draw(backgroundTexture_, Vector2::Zero, Color::White);
        spriteBatch.Draw(plankTexture_, plankPosition_, Color::White);
        spriteBatch.Draw(backTexture_, backPosition_, Color::White);

        spriteBatch.Draw(lineBorderTexture_, linePosition_, Color::White);
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Back",
                               Vector2(backPosition_.X + 55.0f, backPosition_.Y + 5.0f),
                               Color::White);

        spriteBatch.Draw(scrollUpTexture_, scrollUpPosition_, Color::White);
        spriteBatch.Draw(scrollDownTexture_, scrollDownPosition_, Color::White);

        spriteBatch.DrawString(Fonts::HeaderFont(), "Help", titlePosition_, Fonts::TitleColor);

        for (int i = 0; i < MaxLineDisplay; i++) {
            spriteBatch.DrawString(
                Fonts::DescriptionFont(), textLines_[(std::size_t)(startIndex_ + i)],
                Vector2(360.0f, 200.0f + (float)(Fonts::DescriptionFont().getLineSpacingProperty() +
                                                 10) * (float)i),
                Color::Black);
        }

        spriteBatch.End();
    }

private:
    Texture2D backgroundTexture_;

    Texture2D plankTexture_;
    Vector2 plankPosition_;
    Vector2 titlePosition_;

    std::string helpText_ =
        "Welcome, hero!  You must meet new comrades, earn necessary "
        "experience, gold, spells, and the equipment required to challenge "
        "and defeat the evil Tamar, who resides in his lair, known as the "
        "Unspoken Tower.  Be wary!  The Unspoken Tower is filled with "
        "monstrosities that only the most hardened of heroes could possibly "
        "face.  Good luck!";

    std::vector<std::string> textLines_;

    Texture2D scrollUpTexture_;
    const Vector2 scrollUpPosition_{980.0f, 200.0f};
    Texture2D scrollDownTexture_;
    const Vector2 scrollDownPosition_{980.0f, 460.0f};

    Texture2D lineBorderTexture_;
    const Vector2 linePosition_{200.0f, 570.0f};

    Texture2D backTexture_;
    const Vector2 backPosition_{225.0f, 610.0f};

    int startIndex_ = 0;
    static constexpr int MaxLineDisplay = 7;
};

} // namespace RolePlaying
