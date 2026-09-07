#pragma once

// ControlsScreen.hpp -- C++ port of MenuScreens/ControlsScreen.cs.
//
// The original's `#if !XBOX` blocks are kept unconditionally: this port has no Xbox
// configuration, so the Windows arm is the only one that exists.

#include <array>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/TimeSpan.hpp"

#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Displays the in-game controls to the user.
class ControlsScreen : public GameScreen {
public:
    ControlsScreen() {
        SetTransitionOnTime(System::TimeSpan::FromSeconds(0.5));

        chartLine1Position_ = keyboardPosition_.X + 30.0f;
        chartLine2Position_ = keyboardPosition_.X + 340.0f;
        chartLine3Position_ = keyboardPosition_.X + 510.0f;
        chartLine4Position_ = keyboardPosition_.X + 670.0f;

        isShowControlPad_ = true;
    }

    void LoadContent() override {
        keyboardInfo_.selectedIndex = 0;

        const int leftStringsPosition = 450;
        const int rightStringPosition = 818;

        // Set the data for gamepad control to display
        SetLeft(0, "Page Left", leftStringsPosition, 170.0f);
        SetLeft(1, "N/A", leftStringsPosition, 220.0f);
        SetLeft(2, "Main Menu", leftStringsPosition, 290.0f);
        SetLeft(3, "Exit Game", leftStringsPosition, 340.0f);
        SetLeft(4, "Navigation", leftStringsPosition, 400.0f);
        SetLeft(5, "Navigation", leftStringsPosition, 455.0f);
        SetLeft(6, "N/A", leftStringsPosition, 510.0f);

        SetRight(0, "Page Right", rightStringPosition, 170.0f);
        SetRight(1, "N/A", rightStringPosition, 230.0f);
        SetRight(2, "Character Management", rightStringPosition, 295.0f);
        SetRight(3, "Back", rightStringPosition, 355.0f);
        SetRight(4, "OK", rightStringPosition, 435.0f);
        SetRight(5, "Drop Gear", rightStringPosition, 510.0f);

        auto& content = GetScreenManager()->getGameProperty().getContentProperty();
        backgroundTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenu");
        keyboardTexture_ = content.Load<Texture2D>("Textures/MainMenu/KeyboardBkgd");
        plankTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenuPlank03");
        backTexture_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        baseBorderTexture_ = content.Load<Texture2D>("Textures/GameScreens/LineBorder");
        controlPadTexture_ = content.Load<Texture2D>("Textures/MainMenu/ControlJoystick");
        scrollUpTexture_ = content.Load<Texture2D>("Textures/GameScreens/ScrollUp");
        scrollDownTexture_ = content.Load<Texture2D>("Textures/GameScreens/ScrollDown");
        rightTriggerButton_ = content.Load<Texture2D>("Textures/Buttons/RightTriggerButton");
        leftTriggerButton_ = content.Load<Texture2D>("Textures/Buttons/LeftTriggerButton");

        plankPosition_.X = (float)(backgroundTexture_.getWidthProperty() / 2 -
                                   plankTexture_.getWidthProperty() / 2);
        plankPosition_.Y = 60.0f;

        rightTriggerPosition_.X = 900.0f;
        rightTriggerPosition_.Y = 50.0f;

        leftTriggerPosition_.X = 320.0f;
        leftTriggerPosition_.Y = 50.0f;

        GameScreen::LoadContent();
    }

    void HandleInput() override {
        // exit the screen
        if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            ExitScreen();
        }
        // toggle between keyboard and gamepad controls
        else if (InputManager::IsActionTriggered(InputManager::Action::PageLeft) ||
                 InputManager::IsActionTriggered(InputManager::Action::PageRight)) {
            isShowControlPad_ = !isShowControlPad_;
        }
        // scroll through the keyboard controls
        if (isShowControlPad_ == false) {
            if (InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
                if (startIndex_ < (int)InputManager::ActionMaps().size() - MaxActionDisplay) {
                    startIndex_++;
                    keyboardInfo_.selectedIndex++;
                }
            }
            if (InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
                if (startIndex_ > 0) {
                    startIndex_--;
                    keyboardInfo_.selectedIndex--;
                }
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 textPosition = Vector2::Zero;

        spriteBatch.Begin();

        // Draw the background texture
        spriteBatch.Draw(backgroundTexture_, Vector2::Zero, Color::White);

        // Draw the back icon and text
        spriteBatch.Draw(backTexture_, backPosition_, Color::White);
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Back",
                               Vector2(backPosition_.X + 55.0f, backPosition_.Y + 5.0f),
                               Color::White);

        // Draw the plank
        spriteBatch.Draw(plankTexture_, plankPosition_, Color::White);

        // Draw the trigger buttons
        spriteBatch.Draw(leftTriggerButton_, leftTriggerPosition_, Color::White);
        spriteBatch.Draw(rightTriggerButton_, rightTriggerPosition_, Color::White);

        // Draw the base border
        spriteBatch.Draw(baseBorderTexture_, baseBorderPosition_, Color::White);

        // draw the control pad screen
        if (isShowControlPad_) {
            spriteBatch.Draw(controlPadTexture_, controlPosition_, Color::White);

            for (const GamePadInfo& info : leftStrings_) {
                spriteBatch.DrawString(Fonts::DescriptionFont(), info.text, info.textPosition,
                                       Color::Black);
            }

            for (const GamePadInfo& info : rightStrings_) {
                spriteBatch.DrawString(Fonts::DescriptionFont(), info.text, info.textPosition,
                                       Color::Black);
            }

            // Near left trigger
            spriteBatch.DrawString(
                Fonts::PlayerStatisticsFont(), "Keyboard",
                Vector2(leftTriggerPosition_.X +
                            ((float)leftTriggerButton_.getWidthProperty() -
                             Fonts::PlayerStatisticsFont().MeasureString("Keyboard").X) / 2.0f,
                        rightTriggerPosition_.Y + 85.0f),
                Color::Black);

            // Near right trigger
            spriteBatch.DrawString(
                Fonts::PlayerStatisticsFont(), "Keyboard",
                Vector2(rightTriggerPosition_.X +
                            ((float)rightTriggerButton_.getWidthProperty() -
                             Fonts::PlayerStatisticsFont().MeasureString("Keyboard").X) / 2.0f,
                        rightTriggerPosition_.Y + 85.0f),
                Color::Black);

            // Draw the title text
            titlePosition_.X = plankPosition_.X + ((float)plankTexture_.getWidthProperty() -
                                                   Fonts::HeaderFont().MeasureString("Gamepad").X) /
                                                      2.0f;
            titlePosition_.Y = plankPosition_.Y + ((float)plankTexture_.getHeightProperty() -
                                                   Fonts::HeaderFont().MeasureString("Gamepad").Y) /
                                                      2.0f;
            spriteBatch.DrawString(Fonts::HeaderFont(), "Gamepad", titlePosition_,
                                   Fonts::TitleColor);
        } else // draws the keyboard screen
        {
            const float spacing = 47.0f;
            std::string keyboardString;
            const std::vector<InputManager::ActionMap>& totalActionList = InputManager::ActionMaps();

            spriteBatch.Draw(keyboardTexture_, keyboardPosition_, Color::White);
            for (int j = 0, i = startIndex_; i < startIndex_ + MaxActionDisplay; i++, j++) {
                keyboardString = InputManager::GetActionName((InputManager::Action)i);
                textPosition.X = chartLine1Position_ +
                                 ((chartLine2Position_ - chartLine1Position_) -
                                  Fonts::DescriptionFont().MeasureString(keyboardString).X) / 2.0f;
                textPosition.Y = 253.0f + (spacing * (float)j);

                // Draw the action
                spriteBatch.DrawString(Fonts::DescriptionFont(), keyboardString, textPosition,
                                       Color::Black);

                // Draw the key one
                keyboardString =
                    InputManager::KeyName(totalActionList[(std::size_t)i].keyboardKeys[0]);
                textPosition.X = chartLine2Position_ +
                                 ((chartLine3Position_ - chartLine2Position_) -
                                  Fonts::DescriptionFont().MeasureString(keyboardString).X) / 2.0f;
                spriteBatch.DrawString(Fonts::DescriptionFont(), keyboardString, textPosition,
                                       Color::Black);

                // Draw the key two
                if (totalActionList[(std::size_t)i].keyboardKeys.size() > 1) {
                    keyboardString =
                        InputManager::KeyName(totalActionList[(std::size_t)i].keyboardKeys[1]);
                    textPosition.X =
                        chartLine3Position_ + ((chartLine4Position_ - chartLine3Position_) -
                                               Fonts::DescriptionFont().MeasureString(keyboardString).X) /
                                                  2.0f;
                    spriteBatch.DrawString(Fonts::DescriptionFont(), keyboardString, textPosition,
                                           Color::Black);
                } else {
                    textPosition.X = chartLine3Position_ +
                                     ((chartLine4Position_ - chartLine3Position_) -
                                      Fonts::DescriptionFont().MeasureString("---").X) / 2.0f;
                    spriteBatch.DrawString(Fonts::DescriptionFont(), "---", textPosition,
                                           Color::Black);
                }
            }

            // Draw the Action
            actionPosition_.X = chartLine1Position_ +
                                ((chartLine2Position_ - chartLine1Position_) -
                                 Fonts::CaptionFont().MeasureString("Action").X) / 2.0f;
            actionPosition_.Y = 200.0f;
            spriteBatch.DrawString(Fonts::CaptionFont(), "Action", actionPosition_,
                                   Fonts::CaptionColor);

            // Draw the Key 1
            key1Position_.X = chartLine2Position_ +
                              ((chartLine3Position_ - chartLine2Position_) -
                               Fonts::CaptionFont().MeasureString("Key 1").X) / 2.0f;
            key1Position_.Y = 200.0f;
            spriteBatch.DrawString(Fonts::CaptionFont(), "Key 1", key1Position_,
                                   Fonts::CaptionColor);

            // Draw the Key 2
            key2Position_.X = chartLine3Position_ +
                              ((chartLine4Position_ - chartLine3Position_) -
                               Fonts::CaptionFont().MeasureString("Key 2").X) / 2.0f;
            key2Position_.Y = 200.0f;
            spriteBatch.DrawString(Fonts::CaptionFont(), "Key 2", key2Position_,
                                   Fonts::CaptionColor);

            // Near left trigger
            spriteBatch.DrawString(
                Fonts::PlayerStatisticsFont(), "Gamepad",
                Vector2(leftTriggerPosition_.X +
                            ((float)leftTriggerButton_.getWidthProperty() -
                             Fonts::PlayerStatisticsFont().MeasureString("Gamepad").X) / 2.0f,
                        rightTriggerPosition_.Y + 85.0f),
                Color::Black);

            // Near right trigger
            spriteBatch.DrawString(
                Fonts::PlayerStatisticsFont(), "Gamepad",
                Vector2(rightTriggerPosition_.X +
                            ((float)rightTriggerButton_.getWidthProperty() -
                             Fonts::PlayerStatisticsFont().MeasureString("Gamepad").X) / 2.0f,
                        rightTriggerPosition_.Y + 85.0f),
                Color::Black);

            // Draw the title text
            titlePosition_.X =
                plankPosition_.X + ((float)plankTexture_.getWidthProperty() -
                                    Fonts::HeaderFont().MeasureString("Keyboard").X) / 2.0f;
            titlePosition_.Y =
                plankPosition_.Y + ((float)plankTexture_.getHeightProperty() -
                                    Fonts::HeaderFont().MeasureString("Keyboard").Y) / 2.0f;
            spriteBatch.DrawString(Fonts::HeaderFont(), "Keyboard", titlePosition_,
                                   Fonts::TitleColor);

            // Draw the scroll textures
            spriteBatch.Draw(scrollUpTexture_, scrollUpPosition_, Color::White);
            spriteBatch.Draw(scrollDownTexture_, scrollDownPosition_, Color::White);
        }

        spriteBatch.End();
    }

private:
    // Holds the GamePad control info to display
    struct GamePadInfo {
        std::string text;
        Vector2 textPosition;
    };

    // Holds the Keyboard control info to display
    struct KeyboardInfo {
        int selectedIndex = 0;
    };

    void SetLeft(std::size_t index, const std::string& text, int rightEdge, float y) {
        leftStrings_[index].text = text;
        leftStrings_[index].textPosition =
            Vector2((float)rightEdge - Fonts::DescriptionFont().MeasureString(text).X, y);
    }

    void SetRight(std::size_t index, const std::string& text, int leftEdge, float y) {
        rightStrings_[index].text = text;
        rightStrings_[index].textPosition = Vector2((float)leftEdge, y);
    }

    Texture2D backgroundTexture_;
    Texture2D plankTexture_;

    Vector2 plankPosition_;
    Vector2 titlePosition_;
    Vector2 actionPosition_;
    Vector2 key1Position_;
    Vector2 key2Position_;

    Texture2D baseBorderTexture_;
    Vector2 baseBorderPosition_{200.0f, 570.0f};

    Texture2D scrollUpTexture_;
    Texture2D scrollDownTexture_;
    Vector2 scrollUpPosition_{990.0f, 235.0f};
    Vector2 scrollDownPosition_{990.0f, 490.0f};

    Texture2D rightTriggerButton_;
    Texture2D leftTriggerButton_;
    Vector2 rightTriggerPosition_;
    Vector2 leftTriggerPosition_;

    Texture2D controlPadTexture_;
    Vector2 controlPosition_{450.0f, 180.0f};

    Texture2D keyboardTexture_;
    Vector2 keyboardPosition_{305.0f, 185.0f};

    float chartLine1Position_ = 0.0f;
    float chartLine2Position_ = 0.0f;
    float chartLine3Position_ = 0.0f;
    float chartLine4Position_ = 0.0f;

    Texture2D backTexture_;
    const Vector2 backPosition_{225.0f, 610.0f};

    bool isShowControlPad_ = true;

    std::array<GamePadInfo, 7> leftStrings_;
    std::array<GamePadInfo, 6> rightStrings_;
    KeyboardInfo keyboardInfo_;

    int startIndex_ = 0;
    static constexpr int MaxActionDisplay = 6;
};

} // namespace RolePlaying
