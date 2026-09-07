#pragma once

// StoreScreen.hpp -- C++ port of GameScreens/StoreScreen.cs.

#include <memory>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/ArgumentNullException.hpp"

#include "../Data/Map/Store.hpp"
#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "../Session/Session.hpp"
#include "StoreBuyScreen.hpp"
#include "StoreSellScreen.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using RolePlayingGameData::Store;

// Draws the options available in a store - typically to buy or sell gear.
class StoreScreen : public GameScreen {
public:
    // Constructs a new StoreScreen object for the given store.
    explicit StoreScreen(const std::shared_ptr<Store>& store) : store_(store) {
        // check the parameter
        if (store == nullptr) {
            throw System::ArgumentNullException("store");
        }

        SetIsPopup(true);

        titleBarMidPosition_ = Vector2(-Fonts::HeaderFont().MeasureString(store->Name()).X / 2.0f,
                                       0.0f);
        placeTextMid_ = Fonts::ButtonNamesFont().MeasureString("Select");
    }

    void LoadContent() override {
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();

        shopDrawScreen_ = content.Load<Texture2D>("Textures/GameScreens/GameScreenBkgd");
        backButton_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        selectButton_ = content.Load<Texture2D>("Textures/Buttons/AButton");
        highlightItem_ = content.Load<Texture2D>("Textures/GameScreens/HighlightLarge");
        selectionArrow_ = content.Load<Texture2D>("Textures/GameScreens/SelectionArrow");
        fadeTexture_ = content.Load<Texture2D>("Textures/GameScreens/FadeScreen");
        conversationStrip_ = content.Load<Texture2D>("Textures/GameScreens/ConversationStrip");
        goldIcon_ = content.Load<Texture2D>("Textures/GameScreens/GoldIcon");
        plankTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenuPlank03");

        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        screenRect_ = Rectangle(viewport.getXProperty(), viewport.getYProperty(),
                                viewport.getWidthProperty(), viewport.getHeightProperty());
        plankPosition_ = Vector2(
            (float)((viewport.getWidthProperty() - plankTexture_.getWidthProperty()) / 2), 66.0f);
        shopNamePosition_ =
            Vector2(((float)viewport.getWidthProperty() -
                     Fonts::HeaderFont().MeasureString(store_->Name()).X) / 2.0f,
                    90.0f);
    }

    void HandleInput() override {
        // exits the screen
        if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            ExitScreen();
            return;
        }
        // select one of the buttons
        else if (InputManager::IsActionTriggered(InputManager::Action::Ok)) {
            if (currentCursor_ == 0) {
                GetScreenManager()->AddScreen(std::make_shared<StoreBuyScreen>(store_));
            } else if (currentCursor_ == 1) {
                GetScreenManager()->AddScreen(std::make_shared<StoreSellScreen>(store_));
            } else {
                ExitScreen();
            }
            return;
        }
        // move the cursor up
        else if (InputManager::IsActionTriggered(InputManager::Action::MoveCharacterUp)) {
            currentCursor_--;
            if (currentCursor_ < 0) {
                currentCursor_ = 0;
            }
        }
        // move the cursor down
        else if (InputManager::IsActionTriggered(InputManager::Action::MoveCharacterDown)) {
            currentCursor_++;
            if (currentCursor_ > 2) {
                currentCursor_ = 2;
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        // Draw Shop Main Menu
        spriteBatch.Begin();

        // Draw Shop Main Menu Screen
        DrawMainMenu();

        // Draw Buttons
        if (IsActive()) {
            DrawButtons();
        }

        spriteBatch.Draw(plankTexture_, plankPosition_, Color::White);

        // Draw the Title of the Screen
        spriteBatch.DrawString(Fonts::HeaderFont(), store_->Name(), shopNamePosition_,
                               Fonts::TitleColor);

        // Draw Conversation Strip
        spriteBatch.Draw(conversationStrip_, conversationStripPosition_, Color::White);

        // Draw Shop Keeper
        spriteBatch.Draw(*store_->ShopkeeperTexture, shopKeeperPosition_, Color::White);

        // Draw Shop Info
        spriteBatch.DrawString(Fonts::DescriptionFont(),
                               Fonts::BreakTextIntoLines(store_->WelcomeMessage, 55, 3),
                               welcomeMessagePosition_, Fonts::DescriptionColor);

        spriteBatch.End();
    }

private:
    // Draws the main menu for the store.
    void DrawMainMenu() {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        Vector2 arrowPosition = Vector2::Zero;
        Vector2 highlightPosition = Vector2::Zero;
        Vector2 position = textPosition_;

        // Draw faded screen
        spriteBatch.Draw(fadeTexture_, screenRect_, Color::White);

        spriteBatch.Draw(shopDrawScreen_, screenRect_, Color::White);

        arrowPosition.X = textPosition_.X - selectionArrowOffset_.X;
        arrowPosition.Y = textPosition_.Y - selectionArrowOffset_.Y;

        highlightPosition.X = textPosition_.X - highlightItemOffset_.X;
        highlightPosition.Y = textPosition_.Y - highlightItemOffset_.Y;

        // "Buy" is highlighted
        if (currentCursor_ == 0) {
            spriteBatch.Draw(highlightItem_, highlightPosition, Color::White);
            spriteBatch.Draw(selectionArrow_, arrowPosition, Color::White);
            spriteBatch.DrawString(Fonts::GearInfoFont(), "Buy", position, Fonts::HighlightColor);

            position.Y += (float)Interval;
            spriteBatch.DrawString(Fonts::GearInfoFont(), "Sell", position, Fonts::DisplayColor);

            position.Y += (float)Interval;
            spriteBatch.DrawString(Fonts::GearInfoFont(), "Leave", position, Fonts::DisplayColor);
        }
        // "Sell" is highlighted
        else if (currentCursor_ == 1) {
            position = textPosition_;
            spriteBatch.DrawString(Fonts::GearInfoFont(), "Buy", position, Fonts::DisplayColor);

            highlightPosition.Y += (float)Interval;
            arrowPosition.Y += (float)Interval;
            position.Y += (float)Interval;

            spriteBatch.Draw(highlightItem_, highlightPosition, Color::White);
            spriteBatch.Draw(selectionArrow_, arrowPosition, Color::White);
            spriteBatch.DrawString(Fonts::GearInfoFont(), "Sell", position, Fonts::HighlightColor);
            position.Y += (float)Interval;
            spriteBatch.DrawString(Fonts::GearInfoFont(), "Leave", position, Fonts::DisplayColor);
        }
        // "Leave" is highlighted
        else if (currentCursor_ == 2) {
            position = textPosition_;
            spriteBatch.DrawString(Fonts::GearInfoFont(), "Buy", position, Fonts::DisplayColor);

            position.Y += (float)Interval;
            spriteBatch.DrawString(Fonts::GearInfoFont(), "Sell", position, Fonts::DisplayColor);

            highlightPosition.Y += (float)(Interval + Interval);
            arrowPosition.Y += (float)(Interval + Interval);
            position.Y += (float)Interval;

            spriteBatch.Draw(highlightItem_, highlightPosition, Color::White);
            spriteBatch.Draw(selectionArrow_, arrowPosition, Color::White);
            spriteBatch.DrawString(Fonts::GearInfoFont(), "Leave", position,
                                   Fonts::HighlightColor);
        }
    }

    // Draws the buttons.
    void DrawButtons() {
        if (!IsActive()) {
            return;
        }

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        Vector2 position;

        // Draw Back Button
        spriteBatch.Draw(backButton_, backButtonPosition_, Color::White);

        // Draw Back Text
        position = backButtonPosition_;
        position.X += (float)backButton_.getWidthProperty() + 10.0f;
        position.Y += 5.0f;
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Back", position, Color::White);

        // Draw Select Button
        spriteBatch.Draw(selectButton_, selectButtonPosition_, Color::White);

        // Draw Select Text
        position = selectButtonPosition_;
        position.X -= placeTextMid_.X + 10.0f;
        position.Y += 5.0f;
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Select", position, Color::White);

        // Draw Gold Text
        spriteBatch.DrawString(Fonts::ButtonNamesFont(),
                               Fonts::GetGoldString(Session::GetParty()->PartyGold()),
                               partyGoldPosition_, Color::White);

        // Draw Gold Icon
        spriteBatch.Draw(goldIcon_, goldIconPosition_, Color::White);
    }

    std::shared_ptr<Store> store_;

    Texture2D shopDrawScreen_;
    Texture2D selectButton_;
    Texture2D backButton_;
    Texture2D highlightItem_;
    Texture2D selectionArrow_;
    Texture2D conversationStrip_;
    Texture2D plankTexture_;
    Texture2D fadeTexture_;
    Texture2D goldIcon_;

    const Vector2 textPosition_{620.0f, 250.0f};
    const Vector2 backButtonPosition_{80.0f, 640.0f};
    const Vector2 selectButtonPosition_{1150.0f, 640.0f};
    const Vector2 partyGoldPosition_{565.0f, 648.0f};
    const Vector2 shopKeeperPosition_{290.0f, 370.0f};
    const Vector2 welcomeMessagePosition_{470.0f, 460.0f};
    const Vector2 conversationStripPosition_{240.0f, 405.0f};
    const Vector2 goldIconPosition_{490.0f, 640.0f};
    const Vector2 highlightItemOffset_{400.0f, 20.0f};
    const Vector2 selectionArrowOffset_{100.0f, 16.0f};

    Vector2 shopNamePosition_;
    Vector2 plankPosition_;
    Vector2 titleBarMidPosition_;
    Vector2 placeTextMid_;
    Rectangle screenRect_;

    int currentCursor_ = 0;
    static constexpr int Interval = 50;
};

} // namespace RolePlaying
