#pragma once

// RewardsScreen.hpp -- C++ port of GameScreens/RewardsScreen.cs.

#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Int32.hpp"

#include "../AudioManager.hpp"
#include "../Data/Gear/Gear.hpp"
#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "../Session/Session.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using RolePlayingGameData::Gear;

// Displays the rewards earned by the party, from a quest or combat.
class RewardsScreen : public GameScreen {
public:
    enum class RewardScreenMode {
        Quest,
        Combat,
    };

    // Creates a new RewardsScreen object.
    RewardsScreen(RewardScreenMode mode, int experienceReward, int goldReward,
                  std::vector<std::shared_ptr<Gear>> gearReward)
        : mode_(mode), experienceReward_(experienceReward), goldReward_(goldReward),
          gearReward_(std::move(gearReward)) {
        SetIsPopup(true);

        maxLines_ = 3;
        lineSpacing_ = 74;

        startIndex_ = 0;
        endIndex_ = maxLines_;

        if (endIndex_ > (int)gearReward_.size()) {
            endIndex_ = (int)gearReward_.size();
        }

        // play the appropriate music
        switch (mode_) {
        case RewardScreenMode::Combat:
            // play the combat-victory music
            AudioManager::PushMusic("WinTheme");
            break;

        case RewardScreenMode::Quest:
            // play the quest-complete music
            AudioManager::PushMusic("QuestComplete");
            break;
        }

        Exiting += [](System::Object*, const System::EventArgs&) { AudioManager::PopMusic(); };
    }

    void LoadContent() override {
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();
        backTexture_ = content.Load<Texture2D>("Textures/GameScreens/PopupScreen");
        selectIconTexture_ = content.Load<Texture2D>("Textures/Buttons/AButton");
        scrollUpTexture_ = content.Load<Texture2D>("Textures/GameScreens/ScrollUp");
        scrollDownTexture_ = content.Load<Texture2D>("Textures/GameScreens/ScrollDown");
        lineTexture_ = content.Load<Texture2D>("Textures/GameScreens/SeparationLine");
        fadeTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");

        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        fadeDest_ = Rectangle(viewport.getXProperty(), viewport.getYProperty(),
                              viewport.getWidthProperty(), viewport.getHeightProperty());
        backgroundPosition_.X =
            (float)((viewport.getWidthProperty() - backTexture_.getWidthProperty()) / 2);
        backgroundPosition_.Y =
            (float)((viewport.getHeightProperty() - backTexture_.getHeightProperty()) / 2);

        screenSize_ =
            Vector2((float)viewport.getWidthProperty(), (float)viewport.getHeightProperty());

        selectIconPosition_.X = screenSize_.X / 2.0f + 260.0f;
        selectIconPosition_.Y = backgroundPosition_.Y + 530.0f;
        selectPosition_.X = selectIconPosition_.X -
                            Fonts::ButtonNamesFont().MeasureString(selectString_).X - 10.0f;
        selectPosition_.Y = backgroundPosition_.Y + 530.0f;

        textPosition_ = backgroundPosition_ + Vector2(335.0f, 320.0f);
        iconPosition_ = backgroundPosition_ + Vector2(155.0f, 303.0f);
        linePosition_ = backgroundPosition_ + Vector2(142.0f, 285.0f);

        scrollUpPosition_ = backgroundPosition_ + Vector2(810.0f, 300.0f);
        scrollDownPosition_ = backgroundPosition_ + Vector2(810.0f, 480.0f);

        xpAwardPosition_ = backgroundPosition_ + Vector2(160.0f, 180.0f);
        goldAwardPosition_ = backgroundPosition_ + Vector2(160.0f, 210.0f);
        itemAwardPosition_ = backgroundPosition_ + Vector2(160.0f, 240.0f);
    }

    void HandleInput() override {
        // exit the screen
        if (InputManager::IsActionTriggered(InputManager::Action::Ok) ||
            InputManager::IsActionTriggered(InputManager::Action::Back)) {
            ExitScreen();
            // give the rewards to the party
            Session::GetParty()->AddPartyGold(goldReward_);
            for (const std::shared_ptr<Gear>& gear : gearReward_) {
                Session::GetParty()->AddToInventory(gear, 1);
            }
            Session::GetParty()->GiveExperience(experienceReward_);
        }
        // Scroll up
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
            if (startIndex_ > 0) {
                startIndex_--;
                endIndex_--;
            }
        }
        // Scroll down
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
            if (startIndex_ < (int)gearReward_.size() - maxLines_) {
                endIndex_++;
                startIndex_++;
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        Vector2 currentIconPosition = iconPosition_;
        Vector2 currentTextPosition = textPosition_;
        Vector2 currentlinePosition = linePosition_;

        switch (mode_) {
        case RewardScreenMode::Quest:
            titleText_ = "Quest Complete";
            break;

        case RewardScreenMode::Combat:
            titleText_ = "Combat Won";
            break;
        }
        titlePosition_.X =
            (screenSize_.X - Fonts::HeaderFont().MeasureString(titleText_).X) / 2.0f;
        titlePosition_.Y = backgroundPosition_.Y + (float)lineSpacing_;

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        spriteBatch.Begin();

        // Draw the fading screen
        spriteBatch.Draw(fadeTexture_, fadeDest_, Color::White);

        // Draw the popup background
        spriteBatch.Draw(backTexture_, backgroundPosition_, Color::White);

        // Draw the title
        spriteBatch.DrawString(Fonts::HeaderFont(), titleText_, titlePosition_,
                               Fonts::TitleColor);

        // Draw the experience points awarded
        spriteBatch.DrawString(Fonts::GearInfoFont(),
                               "XP Awarded :        " + System::Int32::ToString(experienceReward_),
                               xpAwardPosition_, Fonts::CountColor);

        // Draw the gold points awarded
        spriteBatch.DrawString(Fonts::GearInfoFont(),
                               "Gold Awarded :      " + Fonts::GetGoldString(goldReward_),
                               goldAwardPosition_, Fonts::CountColor);

        // Draw the items awarded
        spriteBatch.DrawString(Fonts::GearInfoFont(), "Items Awarded :", itemAwardPosition_,
                               Fonts::CountColor);

        // Draw horizontal divider lines
        for (int i = 0; i <= maxLines_; i++) {
            spriteBatch.Draw(lineTexture_, currentlinePosition, Color::White);
            currentlinePosition.Y += (float)lineSpacing_;
        }

        // Draw the item details
        for (int i = startIndex_; i < endIndex_; i++) {
            // Draw the item icon
            gearReward_[(std::size_t)i]->DrawIcon(spriteBatch, currentIconPosition);

            // Draw the item name
            spriteBatch.DrawString(Fonts::GearInfoFont(), gearReward_[(std::size_t)i]->Name,
                                   currentTextPosition, Fonts::CountColor);

            // Increment the position to the next line
            currentTextPosition.Y += (float)lineSpacing_;
            currentIconPosition.Y += (float)lineSpacing_;
        }
        // Draw the scroll buttons
        spriteBatch.Draw(scrollUpTexture_, scrollUpPosition_, Color::White);
        spriteBatch.Draw(scrollDownTexture_, scrollDownPosition_, Color::White);

        // Draw the select button and its corresponding text
        spriteBatch.Draw(selectIconTexture_, selectIconPosition_, Color::White);
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), selectString_, selectPosition_,
                               Color::White);
        spriteBatch.End();
    }

private:
    // The mode of this screen.
    RewardScreenMode mode_;

    int experienceReward_;
    int goldReward_;
    std::vector<std::shared_ptr<Gear>> gearReward_;

    Texture2D backTexture_;
    Texture2D selectIconTexture_;
    Texture2D lineTexture_;
    Texture2D scrollUpTexture_;
    Texture2D scrollDownTexture_;
    Texture2D fadeTexture_;

    Vector2 backgroundPosition_;
    Vector2 textPosition_;
    Vector2 iconPosition_;
    Vector2 linePosition_;
    Vector2 selectPosition_;
    Vector2 selectIconPosition_;
    Vector2 screenSize_;
    Vector2 titlePosition_;
    Vector2 scrollUpPosition_;
    Vector2 scrollDownPosition_;
    Vector2 xpAwardPosition_;
    Vector2 goldAwardPosition_;
    Vector2 itemAwardPosition_;
    Rectangle fadeDest_;

    std::string titleText_;
    const std::string selectString_ = "Continue";

    // Starting index of the list to be displayed.
    int startIndex_ = 0;

    // Ending index of the list to be displayed.
    int endIndex_ = 0;

    // Maximum number of lines to draw in the screen.
    int maxLines_ = 0;

    // Vertical spacing between each line.
    int lineSpacing_ = 0;
};

} // namespace RolePlaying
