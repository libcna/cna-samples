#pragma once

// DialogueScreen.hpp -- C++ port of GameScreens/DialogueScreen.cs.

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Display of conversation dialog between the player and the npc.
class DialogueScreen : public GameScreen {
public:
    // Construct a new DialogueScreen object.
    DialogueScreen() { SetIsPopup(true); }

    // The title text shown at the top of the screen.
    const std::string& TitleText() const { return titleText_; }
    void SetTitleText(const std::string& value) { titleText_ = value; }

    // The dialogue shown in the main portion of this dialog.
    const std::string& DialogueText() const { return dialogueText_; }
    void SetDialogueText(const std::string& value) {
        // trim the new value
        std::string trimmedValue = Trim(value);
        // if it's a match for what we already have, then this is trivial
        if (dialogueText_ == trimmedValue) {
            return;
        }
        // assign the new value
        dialogueText_ = trimmedValue;
        // break the text into lines
        if (dialogueText_.empty()) {
            dialogueList_.clear();
        } else {
            dialogueList_ =
                Fonts::BreakTextIntoList(dialogueText_, Fonts::DescriptionFont(), MaxWidth);
        }
        // set which lines are drawn
        startIndex_ = 0;
        endIndex_ = DrawMaxLines;
        if (endIndex_ > (int)dialogueList_.size()) {
            dialogueStartPosition_ =
                Vector2(271.0f, 375.0f - (float)(((int)dialogueList_.size() - startIndex_) *
                                                 Fonts::DescriptionFont().getLineSpacingProperty() /
                                                 2));
            endIndex_ = (int)dialogueList_.size();
        } else {
            dialogueStartPosition_ = Vector2(271.0f, 225.0f);
        }
    }

    // The text shown next to the A button, if any.
    const std::string& SelectText() const { return selectText_; }
    void SetSelectText(const std::string& value) {
        if (selectText_ != value) {
            selectText_ = value;
            selectPosition_.X = selectButtonPosition_.X -
                                Fonts::ButtonNamesFont().MeasureString(selectText_).X - 10.0f;
            selectPosition_.Y = selectButtonPosition_.Y;
        }
    }

    // The text shown next to the B button, if any.
    const std::string& BackText() const { return backText_; }
    void SetBackText(const std::string& value) { backText_ = value; }

    void LoadContent() override {
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();

        fadeTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");
        backgroundTexture_ = content.Load<Texture2D>("Textures/GameScreens/PopupScreen");
        scrollTexture_ = content.Load<Texture2D>("Textures/GameScreens/ScrollButtons");
        selectButtonTexture_ = content.Load<Texture2D>("Textures/Buttons/AButton");
        backButtonTexture_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        lineTexture_ = content.Load<Texture2D>("Textures/GameScreens/SeparationLine");

        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();

        backgroundPosition_.X =
            (float)((viewport.getWidthProperty() - backgroundTexture_.getWidthProperty()) / 2);
        backgroundPosition_.Y =
            (float)((viewport.getHeightProperty() - backgroundTexture_.getHeightProperty()) / 2);

        selectButtonPosition_.X = (float)(viewport.getWidthProperty() / 2 + 260);
        selectButtonPosition_.Y = backgroundPosition_.Y + 530.0f;
        selectPosition_.X = selectButtonPosition_.X -
                            Fonts::ButtonNamesFont().MeasureString(selectText_).X - 10.0f;
        selectPosition_.Y = selectButtonPosition_.Y;

        backPosition_.X = (float)(viewport.getWidthProperty() / 2) - 250.0f;
        backPosition_.Y = backgroundPosition_.Y + 530.0f;
        backButtonPosition_.X =
            backPosition_.X - (float)backButtonTexture_.getWidthProperty() - 10.0f;
        backButtonPosition_.Y = backPosition_.Y;

        scrollPosition_ = backgroundPosition_ + Vector2(820.0f, 200.0f);

        topLinePosition_.X =
            (float)(viewport.getWidthProperty() - lineTexture_.getWidthProperty()) / 2.0f - 30.0f;
        topLinePosition_.Y = 200.0f;

        bottomLinePosition_.X = topLinePosition_.X;
        bottomLinePosition_.Y = 550.0f;

        titlePosition_.X = ((float)viewport.getWidthProperty() -
                            Fonts::HeaderFont().MeasureString(titleText_).X) / 2.0f;
        titlePosition_.Y = backgroundPosition_.Y + 70.0f;
    }

    void HandleInput() override {
        // Press Select or Back
        if (InputManager::IsActionTriggered(InputManager::Action::Ok) ||
            InputManager::IsActionTriggered(InputManager::Action::Back)) {
            ExitScreen();
            return;
        }

        // Scroll up
        if (InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
            if (startIndex_ > 0) {
                startIndex_--;
                endIndex_--;
            }
        }
        // Scroll down
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
            if (startIndex_ < (int)dialogueList_.size() - DrawMaxLines) {
                endIndex_++;
                startIndex_++;
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        Vector2 textPosition = dialogueStartPosition_;

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        spriteBatch.Begin();

        // draw the fading screen
        spriteBatch.Draw(fadeTexture_, Rectangle(0, 0, 1280, 720), Color::White);

        // draw popup background
        spriteBatch.Draw(backgroundTexture_, backgroundPosition_, Color::White);

        // draw the top line
        spriteBatch.Draw(lineTexture_, topLinePosition_, Color::White);

        // draw the bottom line
        spriteBatch.Draw(lineTexture_, bottomLinePosition_, Color::White);

        // draw scrollbar
        spriteBatch.Draw(scrollTexture_, scrollPosition_, Color::White);

        // draw title
        spriteBatch.DrawString(Fonts::HeaderFont(), titleText_, titlePosition_, Fonts::CountColor);

        // draw the dialogue
        for (int i = startIndex_; i < endIndex_; i++) {
            spriteBatch.DrawString(Fonts::DescriptionFont(), dialogueList_[(std::size_t)i],
                                   textPosition, Fonts::CountColor);
            textPosition.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        }

        // draw the Back button and adjoining text
        if (!backText_.empty()) {
            spriteBatch.DrawString(Fonts::ButtonNamesFont(), backText_, backPosition_,
                                   Color::White);
            spriteBatch.Draw(backButtonTexture_, backButtonPosition_, Color::White);
        }

        // draw the Select button and adjoining text
        if (!selectText_.empty()) {
            selectPosition_.X = selectButtonPosition_.X -
                                Fonts::ButtonNamesFont().MeasureString(selectText_).X - 10.0f;
            selectPosition_.Y = selectButtonPosition_.Y;
            spriteBatch.DrawString(Fonts::ButtonNamesFont(), selectText_, selectPosition_,
                                   Color::White);
            spriteBatch.Draw(selectButtonTexture_, selectButtonPosition_, Color::White);
        }

        spriteBatch.End();
    }

private:
    // C#'s string.Trim(), which the dialogue setter applies to its input.
    static std::string Trim(const std::string& value) {
        std::size_t first = value.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return std::string();
        std::size_t last = value.find_last_not_of(" \t\r\n");
        return value.substr(first, last - first + 1);
    }

    Texture2D backgroundTexture_;
    Vector2 backgroundPosition_;
    Texture2D fadeTexture_;

    Texture2D selectButtonTexture_;
    Vector2 selectPosition_;
    Vector2 selectButtonPosition_;

    Vector2 backPosition_;
    Texture2D backButtonTexture_;
    Vector2 backButtonPosition_;

    Texture2D scrollTexture_;
    Vector2 scrollPosition_;

    Texture2D lineTexture_;
    Vector2 topLinePosition_;
    Vector2 bottomLinePosition_;

    Vector2 titlePosition_;
    Vector2 dialogueStartPosition_;

    std::string titleText_;
    std::string dialogueText_;
    std::vector<std::string> dialogueList_;
    std::string selectText_ = "Continue";
    std::string backText_ = "Back";

    // Maximum width of each line in pixels.
    static constexpr int MaxWidth = 705;

    // Maximum number of lines to draw in the screen.
    static constexpr int DrawMaxLines = 13;

    // Starting index of the list to be displayed.
    int startIndex_ = 0;

    // Ending index of the list to be displayed.
    int endIndex_ = DrawMaxLines;
};

} // namespace RolePlaying
