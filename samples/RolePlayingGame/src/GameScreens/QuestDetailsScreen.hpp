#pragma once

// QuestDetailsScreen.hpp -- C++ port of GameScreens/QuestDetailsScreen.cs.

#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Int32.hpp"

#include "../Data/Quests/Quest.hpp"
#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using RolePlayingGameData::Quest;

// Display the details of a particular quest.
class QuestDetailsScreen : public GameScreen {
public:
    // Creates a new QuestDetailsScreen object.
    explicit QuestDetailsScreen(const std::shared_ptr<Quest>& quest) : quest_(quest) {
        // check the parameter
        if (quest == nullptr) {
            throw System::ArgumentNullException("quest");
        }
        SetIsPopup(true);

        maxLines_ = 13;

        textPosition_.X = 261.0f;

        AddStrings(quest_->Name,
                   Fonts::BreakTextIntoList(quest_->Description, Fonts::DescriptionFont(), 715),
                   GetRequirements(*quest_));
    }

    void LoadContent() override {
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();
        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();

        backgroundTexture_ = content.Load<Texture2D>("Textures/GameScreens/PopupScreen");
        backIconTexture_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        scrollTexture_ = content.Load<Texture2D>("Textures/GameScreens/ScrollButtons");
        lineTexture_ = content.Load<Texture2D>("Textures/GameScreens/SeparationLine");
        fadeTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");

        // Get the screen positions
        screenSize_ = Vector2((float)viewport.getWidthProperty(), (float)viewport.getHeightProperty());
        fadeDest_ = Rectangle(viewport.getXProperty(), viewport.getYProperty(),
                              viewport.getWidthProperty(), viewport.getHeightProperty());

        backgroundPosition_ = Vector2(
            (float)((viewport.getWidthProperty() - backgroundTexture_.getWidthProperty()) / 2),
            (float)((viewport.getHeightProperty() - backgroundTexture_.getHeightProperty()) / 2));
        scrollPosition_ = backgroundPosition_ + Vector2(820.0f, 200.0f);

        titlePosition_ =
            Vector2((screenSize_.X - Fonts::HeaderFont().MeasureString(titleString_).X) / 2.0f,
                    backgroundPosition_.Y + 70.0f);

        backTextPosition_ =
            Vector2(screenSize_.X / 2.0f - 250.0f, backgroundPosition_.Y + 530.0f);
        backIconPosition_ = Vector2(
            backTextPosition_.X - (float)backIconTexture_.getWidthProperty() - 10.0f,
            backTextPosition_.Y);

        topLinePosition_ = Vector2(
            (float)(viewport.getWidthProperty() - lineTexture_.getWidthProperty()) / 2.0f - 30.0f,
            200.0f);
        bottomLinePosition_ = Vector2(topLinePosition_.X, 550.0f);
    }

    void HandleInput() override {
        // exit the screen
        if (InputManager::IsActionTriggered(InputManager::Action::Back) ||
            InputManager::IsActionTriggered(InputManager::Action::Ok)) {
            ExitScreen();
            return;
        }
        // scroll up
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
            if (startIndex_ > 0) {
                startIndex_--;
                endIndex_--;
            }
        }
        // scroll Down
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
            if (endIndex_ < (int)currentDialogue_.size()) {
                startIndex_++;
                endIndex_++;
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        Vector2 dialoguePosition = textPosition_;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        spriteBatch.Begin();

        // Draw the fading screen
        spriteBatch.Draw(fadeTexture_, fadeDest_, Color::White);

        // Draw the popup background
        spriteBatch.Draw(backgroundTexture_, backgroundPosition_, Color::White);

        // Draw the top line
        spriteBatch.Draw(lineTexture_, topLinePosition_, Color::White);

        // Draw the bottom line
        spriteBatch.Draw(lineTexture_, bottomLinePosition_, Color::White);

        // Draw the scrollbar
        spriteBatch.Draw(scrollTexture_, scrollPosition_, Color::White);

        // Draw the Back button
        spriteBatch.Draw(backIconTexture_, backIconPosition_, Color::White);
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Back", backTextPosition_, Color::White);

        // Draw the title
        spriteBatch.DrawString(Fonts::HeaderFont(), titleString_, titlePosition_, Fonts::TitleColor);

        // Draw the information dialog
        for (int i = startIndex_; i < endIndex_; i++) {
            const Line& line = currentDialogue_[(std::size_t)i];
            dialoguePosition.X =
                (float)(int)((screenSize_.X - line.font->MeasureString(line.text).X) / 2.0f) - 20.0f;

            spriteBatch.DrawString(*line.font, line.text, dialoguePosition, line.color);
            dialoguePosition.Y += (float)line.font->getLineSpacingProperty();
        }

        spriteBatch.End();
    }

private:
    // A line of text with its own color and font.
    struct Line {
        std::string text;
        Color color;
        SpriteFont* font = nullptr;
    };

    // Add strings to list of lines
    void AddStrings(const std::string& name, const std::vector<std::string>& description,
                    const std::vector<Line>& requirements) {
        Line line;

        line.color = headerColor_;
        line.font = &Fonts::DescriptionFont();

        // Title text
        titleString_ = name;
        titlePosition_.X =
            (screenSize_.X - Fonts::HeaderFont().MeasureString(titleString_).X) / 2.0f;
        titlePosition_.Y = backgroundPosition_.Y + 70.0f;

        currentDialogue_.clear();
        line.text = "Description";
        currentDialogue_.push_back(line);
        for (const std::string& str : description) {
            line.text = str;
            line.color = textColor_;
            currentDialogue_.push_back(line);
        }
        for (const Line& str : requirements) {
            currentDialogue_.push_back(str);
        }
        // Set the start index and end index
        startIndex_ = 0;
        endIndex_ = maxLines_;
        if (endIndex_ > (int)currentDialogue_.size()) {
            textPosition_.Y = 375.0f;
            for (const Line& str : currentDialogue_) {
                textPosition_.Y -= (float)(str.font->getLineSpacingProperty() / 2);
            }

            endIndex_ = (int)currentDialogue_.size();
        } else {
            textPosition_.Y = 225.0f;
        }
    }

    // Get the quest requirements
    std::vector<Line> GetRequirements(const Quest& quest) {
        std::vector<Line> reqdList;
        Line reqd;
        int currentCount = 0;
        int totalCount = 0;
        std::vector<std::string> dialog;

        reqd.font = &Fonts::DescriptionFont();

        // Add Monster Requirements
        if (!quest.MonsterRequirements.empty()) {
            reqd.color = headerColor_;
            reqd.text.clear();
            reqdList.push_back(reqd);
            reqd.text = "Monster Progress";
            reqdList.push_back(reqd);

            for (const auto& requirement : quest.MonsterRequirements) {
                reqd.color = textColor_;
                currentCount = requirement->CompletedCount;
                totalCount = requirement->Count;
                reqd.text = requirement->Content->Name() + " = " +
                            System::Int32::ToString(currentCount) + " / " +
                            System::Int32::ToString(totalCount);

                if (currentCount == totalCount) {
                    reqd.color = Color::Red;
                }
                reqdList.push_back(reqd);
            }
        }

        // Add Item Requirements
        if (!quest.GearRequirements.empty()) {
            reqd.color = headerColor_;
            reqd.text.clear();
            reqdList.push_back(reqd);
            reqd.text = "Item Progress";
            reqdList.push_back(reqd);

            for (const auto& requirement : quest.GearRequirements) {
                reqd.color = textColor_;
                currentCount = requirement->CompletedCount;
                totalCount = requirement->Count;
                reqd.text = requirement->Content->Name + " = " +
                            System::Int32::ToString(currentCount) + " / " +
                            System::Int32::ToString(totalCount);
                if (currentCount == totalCount) {
                    reqd.color = Color::Red;
                }
                reqdList.push_back(reqd);
            }
        }

        // Add Current Objective
        reqd.color = headerColor_;
        reqd.text.clear();
        reqdList.push_back(reqd);
        reqd.text = "Current Objective";
        reqdList.push_back(reqd);
        reqd.color = textColor_;

        switch (quest.Stage) {
        case Quest::QuestStage::InProgress:
            dialog =
                Fonts::BreakTextIntoList(quest.ObjectiveMessage, Fonts::DescriptionFont(), 715);
            for (const std::string& text : dialog) {
                reqd.text = text;
                reqdList.push_back(reqd);
            }
            break;

        case Quest::QuestStage::RequirementsMet:
            dialog = Fonts::BreakTextIntoList(quest.DestinationObjectiveMessage,
                                              Fonts::DescriptionFont(), 715);
            for (const std::string& text : dialog) {
                reqd.text = text;
                reqdList.push_back(reqd);
            }
            break;

        case Quest::QuestStage::Completed:
            reqd.font = &Fonts::ButtonNamesFont();
            reqd.color = Color(139, 21, 73);
            reqd.text = "Quest Completed";
            reqdList.push_back(reqd);
            break;

        case Quest::QuestStage::NotStarted:
            break;
        }

        return reqdList;
    }

    std::shared_ptr<Quest> quest_;

    Texture2D backgroundTexture_;
    Texture2D backIconTexture_;
    Texture2D scrollTexture_;
    Texture2D fadeTexture_;
    Texture2D lineTexture_;

    Vector2 backgroundPosition_;
    Vector2 screenSize_;
    Vector2 titlePosition_;
    Vector2 textPosition_;
    Vector2 backTextPosition_;
    Vector2 backIconPosition_;
    Vector2 scrollPosition_;
    Vector2 topLinePosition_;
    Vector2 bottomLinePosition_;
    Rectangle fadeDest_;

    Color headerColor_{128, 6, 6};
    Color textColor_{102, 40, 16};

    std::string titleString_ = "Quest Details";
    std::vector<Line> currentDialogue_;

    int startIndex_ = 0;
    int endIndex_ = 0;
    int maxLines_ = 0;
};

} // namespace RolePlaying
