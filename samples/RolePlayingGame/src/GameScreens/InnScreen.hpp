#pragma once

// InnScreen.hpp -- C++ port of GameScreens/InnScreen.cs.

#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/ArgumentNullException.hpp"
#include "System/Int32.hpp"

#include "../AudioManager.hpp"
#include "../Data/Map/Inn.hpp"
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
using RolePlayingGameData::Inn;

// Displays the options for an inn that the party can stay at.
class InnScreen : public GameScreen {
public:
    // Creates a new InnScreen object.
    explicit InnScreen(const std::shared_ptr<Inn>& inn) : inn_(inn) {
        // check the parameter
        if (inn == nullptr) {
            throw System::ArgumentNullException("inn");
        }

        SetIsPopup(true);

        welcomeMessage_ =
            Fonts::BreakTextIntoList(inn_->WelcomeMessage, Fonts::DescriptionFont(), MaxWidth);
        serviceRenderedMessage_ =
            Fonts::BreakTextIntoList(inn_->PaidMessage, Fonts::DescriptionFont(), MaxWidth);
        noGoldMessage_ = Fonts::BreakTextIntoList(inn_->NotEnoughGoldMessage,
                                                  Fonts::DescriptionFont(), MaxWidth);

        selectionMark_ = 1;
        ChangeDialogue(welcomeMessage_);
    }

    void LoadContent() override {
        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();

        backgroundTexture_ = content.Load<Texture2D>("Textures/GameScreens/GameScreenBkgd");
        plankTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenuPlank03");
        selectIconTexture_ = content.Load<Texture2D>("Textures/Buttons/AButton");
        backIconTexture_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        highlightTexture_ = content.Load<Texture2D>("Textures/GameScreens/HighlightLarge");
        arrowTexture_ = content.Load<Texture2D>("Textures/GameScreens/SelectionArrow");
        conversationTexture_ = content.Load<Texture2D>("Textures/GameScreens/ConversationStrip");
        goldIcon_ = content.Load<Texture2D>("Textures/GameScreens/GoldIcon");
        fadeTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");

        screenRectangle_ = Rectangle(viewport.getXProperty(), viewport.getYProperty(),
                                     viewport.getWidthProperty(), viewport.getHeightProperty());

        plankPosition_ = Vector2(
            (float)((viewport.getWidthProperty() - plankTexture_.getWidthProperty()) / 2), 67.0f);

        backgroundPosition_ = Vector2(
            (float)((viewport.getWidthProperty() - backgroundTexture_.getWidthProperty()) / 2),
            (float)((viewport.getHeightProperty() - backgroundTexture_.getHeightProperty()) / 2));

        namePosition_ =
            Vector2(((float)viewport.getWidthProperty() -
                     Fonts::HeaderFont().MeasureString(inn_->Name()).X) / 2.0f,
                    90.0f);

        selectTextPosition_ = selectIconPosition_;
        selectTextPosition_.X -= Fonts::ButtonNamesFont().MeasureString(selectString_).X + 10.0f;
        selectTextPosition_.Y += 5.0f;

        backTextPosition_ = backIconPosition_;
        backTextPosition_.X += (float)backIconTexture_.getWidthProperty() + 10.0f;
        backTextPosition_.Y += 5.0f;
    }

    void HandleInput() override {
        // exit the screen
        if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            ExitScreen();
            return;
        }
        // move the cursor up
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorUp)) {
            if (selectionMark_ == 2) {
                selectionMark_ = 1;
            }
        }
        // move the cursor down
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorDown)) {
            if (selectionMark_ == 1) {
                selectionMark_ = 2;
            }
        }
        // select an option
        else if (InputManager::IsActionTriggered(InputManager::Action::Ok)) {
            if (selectionMark_ == 1) {
                int partyCharge = GetChargeForParty(*Session::GetParty());
                if (Session::GetParty()->PartyGold() >= partyCharge) {
                    AudioManager::PlayCue("Money");
                    Session::GetParty()->AddPartyGold(-partyCharge);
                    selectionMark_ = 2;
                    ChangeDialogue(serviceRenderedMessage_);
                    HealParty(*Session::GetParty());
                } else {
                    selectionMark_ = 2;
                    ChangeDialogue(noGoldMessage_);
                }
            } else {
                ExitScreen();
                return;
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        Vector2 dialogPosition = informationPosition_;

        spriteBatch.Begin();

        // Draw fade screen
        spriteBatch.Draw(fadeTexture_, screenRectangle_, Color::White);

        // Draw the background
        spriteBatch.Draw(backgroundTexture_, backgroundPosition_, Color::White);
        // Draw the wooden plank
        spriteBatch.Draw(plankTexture_, plankPosition_, Color::White);
        // Draw the select icon
        spriteBatch.Draw(selectIconTexture_, selectIconPosition_, Color::White);
        // Draw the back icon
        spriteBatch.Draw(backIconTexture_, backIconPosition_, Color::White);
        // Draw the inn name on the wooden plank
        spriteBatch.DrawString(Fonts::HeaderFont(), inn_->Name(), namePosition_,
                               Fonts::DisplayColor);

        // Draw the stay and leave option texts based on the current selection
        if (selectionMark_ == 1) {
            spriteBatch.Draw(highlightTexture_, stayHighlightPosition_, Color::White);
            spriteBatch.Draw(arrowTexture_, stayArrowPosition_, Color::White);
            spriteBatch.DrawString(Fonts::GearInfoFont(), stayString_, stayPosition_,
                                   Fonts::HighlightColor);
            spriteBatch.DrawString(Fonts::GearInfoFont(), leaveString_, leavePosition_,
                                   Fonts::DisplayColor);
        } else {
            spriteBatch.Draw(highlightTexture_, leaveHighlightPosition_, Color::White);
            spriteBatch.Draw(arrowTexture_, leaveArrowPosition_, Color::White);
            spriteBatch.DrawString(Fonts::GearInfoFont(), stayString_, stayPosition_,
                                   Fonts::DisplayColor);
            spriteBatch.DrawString(Fonts::GearInfoFont(), leaveString_, leavePosition_,
                                   Fonts::HighlightColor);
        }
        // Draw the amount of gold
        spriteBatch.DrawString(Fonts::ButtonNamesFont(),
                               Fonts::GetGoldString(Session::GetParty()->PartyGold()),
                               goldStringPosition_, Color::White);
        // Draw the select button text
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), selectString_, selectTextPosition_,
                               Color::White);
        // Draw the back button text
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), backString_, backTextPosition_,
                               Color::White);

        // Draw Conversation Strip
        spriteBatch.Draw(conversationTexture_, conversationStripPosition_, Color::White);

        // Draw Shop Keeper
        spriteBatch.Draw(*inn_->ShopkeeperTexture, innKeeperPosition_, Color::White);
        // Draw the cost to stay
        costString_ = "Cost: " +
                      System::Int32::ToString(GetChargeForParty(*Session::GetParty())) + " Gold";
        spriteBatch.DrawString(Fonts::DescriptionFont(), costString_, costPosition_,
                               Color::DarkRed);
        // Draw the innkeeper dialog
        for (int i = 0; i < endIndex_; i++) {
            spriteBatch.DrawString(Fonts::DescriptionFont(), currentDialogue_[(std::size_t)i],
                                   dialogPosition, Color::Black);
            dialogPosition.Y += (float)Fonts::DescriptionFont().getLineSpacingProperty();
        }

        // Draw Gold Icon
        spriteBatch.Draw(goldIcon_, goldIconPosition_, Color::White);

        spriteBatch.End();
    }

private:
    // Change the current dialogue.
    void ChangeDialogue(const std::vector<std::string>& newDialogue) {
        currentDialogue_ = newDialogue;
        endIndex_ = MaxLines;
        if (endIndex_ > (int)currentDialogue_.size()) {
            endIndex_ = (int)currentDialogue_.size();
        }
    }

    // Calculate the charge for the party's stay at the inn.
    int GetChargeForParty(Party& party) const {
        return inn_->ChargePerPlayer * (int)party.Players.size();
    }

    // Heal the party back to their correct values for level + gear.
    void HealParty(Party& party) {
        // reset the statistics for each player
        for (const std::shared_ptr<RolePlayingGameData::Player>& player : party.Players) {
            player->StatisticsModifiers = RolePlayingGameData::StatisticsValue();
        }
    }

    std::shared_ptr<Inn> inn_;

    Texture2D backgroundTexture_;
    Texture2D plankTexture_;
    Texture2D selectIconTexture_;
    Texture2D backIconTexture_;
    Texture2D highlightTexture_;
    Texture2D arrowTexture_;
    Texture2D conversationTexture_;
    Texture2D fadeTexture_;
    Texture2D goldIcon_;

    const Vector2 stayPosition_{620.0f, 250.0f};
    const Vector2 leavePosition_{620.0f, 300.0f};
    const Vector2 costPosition_{470.0f, 450.0f};
    const Vector2 informationPosition_{470.0f, 490.0f};
    const Vector2 selectIconPosition_{1150.0f, 640.0f};
    const Vector2 backIconPosition_{80.0f, 640.0f};
    const Vector2 goldStringPosition_{565.0f, 648.0f};
    const Vector2 stayArrowPosition_{520.0f, 234.0f};
    const Vector2 leaveArrowPosition_{520.0f, 284.0f};
    const Vector2 stayHighlightPosition_{180.0f, 230.0f};
    const Vector2 leaveHighlightPosition_{180.0f, 280.0f};
    const Vector2 innKeeperPosition_{290.0f, 370.0f};
    const Vector2 conversationStripPosition_{210.0f, 405.0f};
    const Vector2 goldIconPosition_{490.0f, 640.0f};
    Vector2 plankPosition_;
    Vector2 backgroundPosition_;
    Vector2 namePosition_;
    Vector2 selectTextPosition_;
    Vector2 backTextPosition_;
    Rectangle screenRectangle_;

    std::vector<std::string> welcomeMessage_;
    std::vector<std::string> serviceRenderedMessage_;
    std::vector<std::string> noGoldMessage_;
    std::vector<std::string> currentDialogue_;
    static constexpr int MaxWidth = 570;
    static constexpr int MaxLines = 3;

    std::string costString_;
    const std::string stayString_ = "Stay";
    const std::string leaveString_ = "Leave";
    const std::string selectString_ = "Select";
    const std::string backString_ = "Leave";

    int selectionMark_ = 0;
    int endIndex_ = 0;
};

} // namespace RolePlaying
