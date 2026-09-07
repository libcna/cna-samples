#pragma once

// MessageBoxScreen.hpp -- C++ port of MenuScreens/MessageBoxScreen.cs.

#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/EventHandler.hpp"
#include "System/TimeSpan.hpp"

#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// A popup message box screen, used to display "are you sure?" confirmation messages.
class MessageBoxScreen : public GameScreen {
public:
    System::EventHandler<System::EventArgs> Accepted;
    System::EventHandler<System::EventArgs> Cancelled;

    // Constructor lets the caller specify the message.
    explicit MessageBoxScreen(const std::string& message) : message_(message) {
        SetIsPopup(true);

        SetTransitionOnTime(System::TimeSpan::FromSeconds(0.2));
        SetTransitionOffTime(System::TimeSpan::FromSeconds(0.2));
    }

    void LoadContent() override {
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();

        backgroundTexture_ = content.Load<Texture2D>("Textures/MainMenu/Confirm");
        backTexture_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        selectTexture_ = content.Load<Texture2D>("Textures/Buttons/AButton");
        loadingBlackTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");

        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        backgroundPosition_ =
            Vector2((float)((viewport.getWidthProperty() - backgroundTexture_.getWidthProperty()) / 2),
                    (float)((viewport.getHeightProperty() - backgroundTexture_.getHeightProperty()) / 2));
        loadingBlackTextureDestination_ =
            Rectangle(viewport.getXProperty(), viewport.getYProperty(), viewport.getWidthProperty(),
                      viewport.getHeightProperty());

        backPosition_ = backgroundPosition_ +
                        Vector2(50.0f, (float)backgroundTexture_.getHeightProperty() - 100.0f);
        selectPosition_ =
            backgroundPosition_ + Vector2((float)backgroundTexture_.getWidthProperty() - 100.0f,
                                          (float)backgroundTexture_.getHeightProperty() - 100.0f);

        confirmPosition_.X =
            backgroundPosition_.X + ((float)backgroundTexture_.getWidthProperty() -
                                     Fonts::HeaderFont().MeasureString("Confirmation").X) / 2.0f;
        confirmPosition_.Y = backgroundPosition_.Y + 47.0f;

        message_ = Fonts::BreakTextIntoLines(message_, 36, 10);
        messagePosition_.X =
            backgroundPosition_.X + (float)(int)(((float)backgroundTexture_.getWidthProperty() -
                                                  Fonts::GearInfoFont().MeasureString(message_).X) /
                                                 2.0f);
        messagePosition_.Y = (backgroundPosition_.Y * 2.0f) - 20.0f;
    }

    // Responds to user input, accepting or cancelling the message box.
    void HandleInput() override {
        if (InputManager::IsActionTriggered(InputManager::Action::Ok)) {
            // Raise the accepted event, then exit the message box.
            Accepted.Raise(this, System::EventArgs());

            ExitScreen();
        } else if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            // Raise the cancelled event, then exit the message box.
            Cancelled.Raise(this, System::EventArgs());

            ExitScreen();
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        spriteBatch.Begin();

        spriteBatch.Draw(loadingBlackTexture_, loadingBlackTextureDestination_, Color::White);
        spriteBatch.Draw(backgroundTexture_, backgroundPosition_, Color::White);
        spriteBatch.Draw(backTexture_, backPosition_, Color::White);
        spriteBatch.Draw(selectTexture_, selectPosition_, Color::White);
        spriteBatch.DrawString(
            Fonts::ButtonNamesFont(), "No",
            Vector2(backPosition_.X + (float)backTexture_.getWidthProperty() + 5.0f,
                    backPosition_.Y + 5.0f),
            Color::White);
        spriteBatch.DrawString(
            Fonts::ButtonNamesFont(), "Yes",
            Vector2(selectPosition_.X - Fonts::ButtonNamesFont().MeasureString("Yes").X,
                    selectPosition_.Y + 5.0f),
            Color::White);
        spriteBatch.DrawString(Fonts::HeaderFont(), "Confirmation", confirmPosition_,
                               Fonts::CountColor);
        spriteBatch.DrawString(Fonts::GearInfoFont(), message_, messagePosition_,
                               Fonts::CountColor);

        spriteBatch.End();
    }

private:
    std::string message_;

    Texture2D backgroundTexture_;
    Vector2 backgroundPosition_;

    Texture2D loadingBlackTexture_;
    Rectangle loadingBlackTextureDestination_;

    Texture2D backTexture_;
    Vector2 backPosition_;

    Texture2D selectTexture_;
    Vector2 selectPosition_;

    Vector2 confirmPosition_, messagePosition_;
};

} // namespace RolePlaying
