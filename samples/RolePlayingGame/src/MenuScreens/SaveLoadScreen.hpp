#pragma once

// SaveLoadScreen.hpp -- C++ port of MenuScreens/SaveLoadScreen.cs.

#include <algorithm>
#include <functional>
#include <memory>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../Fonts.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "../Session/SaveGameDescription.hpp"
#include "../Session/Session.hpp"
#include "MessageBoxScreen.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// Displays a list of existing save games, allowing the user to save, load, or delete.
class SaveLoadScreen : public GameScreen {
public:
    enum class SaveLoadScreenMode { Save, Load };

    // Fired when a save game is selected to load. Loading save games exits multiple screens,
    // so we use events to move backwards.
    std::function<void(const SaveGameDescription&)> LoadingSaveGame;

    // Create a new SaveLoadScreen object.
    explicit SaveLoadScreen(SaveLoadScreenMode mode) : mode_(mode) {
        // refresh the save game descriptions
        Session::RefreshSaveGameDescriptions();
    }

    void LoadContent() override {
        // load the textures
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();
        backgroundTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenu");
        plankTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenuPlank03");
        backTexture_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        selectTexture_ = content.Load<Texture2D>("Textures/Buttons/AButton");
        deleteTexture_ = content.Load<Texture2D>("Textures/Buttons/XButton");
        lineBorderTexture_ = content.Load<Texture2D>("Textures/GameScreens/LineBorder");
        highlightTexture_ = content.Load<Texture2D>("Textures/GameScreens/HighlightLarge");
        arrowTexture_ = content.Load<Texture2D>("Textures/GameScreens/SelectionArrow");

        // calculate the image positions
        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        backgroundPosition_ = Vector2(
            (float)((viewport.getWidthProperty() - backgroundTexture_.getWidthProperty()) / 2),
            (float)((viewport.getHeightProperty() - backgroundTexture_.getHeightProperty()) / 2));
        plankPosition_ =
            backgroundPosition_ + Vector2((float)(backgroundTexture_.getWidthProperty() / 2 -
                                                  plankTexture_.getWidthProperty() / 2),
                                          60.0f);
        backPosition_ = backgroundPosition_ + Vector2(225.0f, 610.0f);
        selectPosition_ = backgroundPosition_ + Vector2(1120.0f, 610.0f);
        lineBorderPosition_ = backgroundPosition_ + Vector2(200.0f, 570.0f);

        // calculate the text positions
        titleTextPosition_ =
            backgroundPosition_ +
            Vector2(plankPosition_.X + ((float)plankTexture_.getWidthProperty() -
                                        Fonts::HeaderFont().MeasureString("Load").X) / 2.0f,
                    plankPosition_.Y + ((float)plankTexture_.getHeightProperty() -
                                        Fonts::HeaderFont().MeasureString("Load").Y) / 2.0f);
        backTextPosition_ = Vector2(backPosition_.X + 55.0f, backPosition_.Y + 5.0f);
        deleteTextPosition_.X += (float)deleteTexture_.getWidthProperty();
        selectTextPosition_ =
            Vector2(selectPosition_.X - Fonts::ButtonNamesFont().MeasureString("Select").X - 5.0f,
                    selectPosition_.Y + 5.0f);

        GameScreen::LoadContent();
    }

    // Respond to user input.
    void HandleInput() override {
        const std::vector<SaveGameDescription>* descriptions = Session::SaveGameDescriptions();

        // handle exiting the screen
        if (InputManager::IsActionTriggered(InputManager::Action::Back)) {
            ExitScreen();
            return;
        }

        // handle selecting a save game
        if (InputManager::IsActionTriggered(InputManager::Action::Ok) && descriptions != nullptr) {
            switch (mode_) {
            case SaveLoadScreenMode::Load:
                if (currentSlot_ >= 0 && currentSlot_ < (int)descriptions->size()) {
                    if (Session::IsActive()) {
                        auto messageBoxScreen = std::make_shared<MessageBoxScreen>(
                            "Are you sure you want to load this game?");
                        messageBoxScreen->Accepted += [this](System::Object*,
                                                             const System::EventArgs&) {
                            ConfirmLoadMessageBoxAccepted();
                        };
                        GetScreenManager()->AddScreen(messageBoxScreen);
                    } else {
                        ConfirmLoadMessageBoxAccepted();
                    }
                }
                break;

            case SaveLoadScreenMode::Save:
                if (currentSlot_ >= 0 && currentSlot_ <= (int)descriptions->size()) {
                    if (currentSlot_ == (int)descriptions->size()) {
                        ConfirmSaveMessageBoxAccepted();
                    } else {
                        auto messageBoxScreen = std::make_shared<MessageBoxScreen>(
                            "Are you sure you want to overwrite this save game?");
                        messageBoxScreen->Accepted += [this](System::Object*,
                                                             const System::EventArgs&) {
                            ConfirmSaveMessageBoxAccepted();
                        };
                        GetScreenManager()->AddScreen(messageBoxScreen);
                    }
                }
                break;
            }
        }
        // handle deletion
        else if (InputManager::IsActionTriggered(InputManager::Action::DropUnEquip) &&
                 descriptions != nullptr) {
            if (currentSlot_ >= 0 && currentSlot_ < (int)descriptions->size()) {
                auto messageBoxScreen =
                    std::make_shared<MessageBoxScreen>("Are you sure you want to delete this save game?");
                messageBoxScreen->Accepted += [this](System::Object*, const System::EventArgs&) {
                    ConfirmDeleteMessageBoxAccepted();
                };
                GetScreenManager()->AddScreen(messageBoxScreen);
            }
        }
        // handle cursor-down
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorDown) &&
                 descriptions != nullptr) {
            int maximumSlot = (int)descriptions->size();
            if (mode_ == SaveLoadScreenMode::Save) {
                maximumSlot = std::min(maximumSlot + 1, Session::MaximumSaveGameDescriptions);
            }
            if (currentSlot_ < maximumSlot - 1) {
                currentSlot_++;
            }
        }
        // handle cursor-up
        else if (InputManager::IsActionTriggered(InputManager::Action::CursorUp) &&
                 descriptions != nullptr) {
            if (currentSlot_ >= 1) {
                currentSlot_--;
            }
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        spriteBatch.Begin();

        spriteBatch.Draw(backgroundTexture_, backgroundPosition_, Color::White);
        spriteBatch.Draw(plankTexture_, plankPosition_, Color::White);
        spriteBatch.Draw(lineBorderTexture_, lineBorderPosition_, Color::White);

        spriteBatch.Draw(backTexture_, backPosition_, Color::White);
        spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Back", backTextPosition_, Color::White);

        spriteBatch.DrawString(Fonts::HeaderFont(),
                               mode_ == SaveLoadScreenMode::Load ? "Load" : "Save",
                               titleTextPosition_, Fonts::TitleColor);

        const std::vector<SaveGameDescription>* descriptions = Session::SaveGameDescriptions();
        if (descriptions != nullptr) {
            for (int i = 0; i < (int)descriptions->size(); i++) {
                Vector2 descriptionTextPosition(
                    295.0f, 200.0f + (float)i * ((float)Fonts::GearInfoFont().getLineSpacingProperty() + 40.0f));
                Color descriptionTextColor = Color::Black;

                // if the save game is selected, draw the highlight color
                if (i == currentSlot_) {
                    descriptionTextColor = Fonts::HighlightColor;
                    spriteBatch.Draw(highlightTexture_,
                                     descriptionTextPosition + Vector2(-100.0f, -23.0f),
                                     Color::White);
                    spriteBatch.Draw(arrowTexture_,
                                     descriptionTextPosition + Vector2(-75.0f, -15.0f),
                                     Color::White);

                    spriteBatch.Draw(deleteTexture_, deletePosition_, Color::White);
                    spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Delete", deleteTextPosition_,
                                           Color::White);

                    spriteBatch.Draw(selectTexture_, selectPosition_, Color::White);
                    spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Select", selectTextPosition_,
                                           Color::White);
                }

                spriteBatch.DrawString(Fonts::GearInfoFont(), (*descriptions)[i].ChapterName,
                                       descriptionTextPosition, descriptionTextColor);
                descriptionTextPosition.X = 650.0f;
                spriteBatch.DrawString(Fonts::GearInfoFont(), (*descriptions)[i].Description,
                                       descriptionTextPosition, descriptionTextColor);
            }

            // if there is space for one, add an empty entry
            if (mode_ == SaveLoadScreenMode::Save &&
                (int)descriptions->size() < Session::MaximumSaveGameDescriptions) {
                int i = (int)descriptions->size();
                Vector2 descriptionTextPosition(
                    295.0f, 200.0f + (float)i * ((float)Fonts::GearInfoFont().getLineSpacingProperty() + 40.0f));
                Color descriptionTextColor = Color::Black;

                // if the save game is selected, draw the highlight color
                if (i == currentSlot_) {
                    descriptionTextColor = Fonts::HighlightColor;
                    spriteBatch.Draw(highlightTexture_,
                                     descriptionTextPosition + Vector2(-100.0f, -23.0f),
                                     Color::White);
                    spriteBatch.Draw(arrowTexture_,
                                     descriptionTextPosition + Vector2(-75.0f, -15.0f),
                                     Color::White);
                    spriteBatch.Draw(selectTexture_, selectPosition_, Color::White);
                    spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Select", selectTextPosition_,
                                           Color::White);
                }

                spriteBatch.DrawString(Fonts::GearInfoFont(), "-------empty------",
                                       descriptionTextPosition, descriptionTextColor);
                descriptionTextPosition.X = 650.0f;
                spriteBatch.DrawString(Fonts::GearInfoFont(), "-----", descriptionTextPosition,
                                       descriptionTextColor);
            }
        }

        // if there are no slots to load, report that
        if (descriptions == nullptr) {
            spriteBatch.DrawString(Fonts::GearInfoFont(), "No Storage Device Available",
                                   Vector2(295.0f, 200.0f), Color::Black);
        } else if (mode_ == SaveLoadScreenMode::Load && descriptions->empty()) {
            spriteBatch.DrawString(Fonts::GearInfoFont(), "No Save Games Available",
                                   Vector2(295.0f, 200.0f), Color::Black);
        }

        spriteBatch.End();
    }

private:
    // Callback for the Save Game confirmation message box.
    void ConfirmSaveMessageBoxAccepted() {
        const std::vector<SaveGameDescription>* descriptions = Session::SaveGameDescriptions();
        if (descriptions == nullptr) return;
        if (currentSlot_ >= 0 && currentSlot_ <= (int)descriptions->size()) {
            if (currentSlot_ == (int)descriptions->size()) {
                Session::SaveSession(nullptr);
            } else {
                Session::SaveSession(&(*descriptions)[currentSlot_]);
            }
            ExitScreen();
        }
    }

    // Callback for the Load Game confirmation message box.
    void ConfirmLoadMessageBoxAccepted() {
        const std::vector<SaveGameDescription>* descriptions = Session::SaveGameDescriptions();
        if (descriptions != nullptr && currentSlot_ >= 0 &&
            currentSlot_ < (int)descriptions->size()) {
            SaveGameDescription selected = (*descriptions)[currentSlot_];
            ExitScreen();
            if (LoadingSaveGame) {
                LoadingSaveGame(selected);
            }
        }
    }

    // Callback for the Delete Game confirmation message box.
    void ConfirmDeleteMessageBoxAccepted() {
        const std::vector<SaveGameDescription>* descriptions = Session::SaveGameDescriptions();
        if (descriptions != nullptr && currentSlot_ >= 0 &&
            currentSlot_ < (int)descriptions->size()) {
            Session::DeleteSaveGame((*descriptions)[currentSlot_]);
        }
    }

    // The mode of this screen.
    SaveLoadScreenMode mode_;

    // The current selected slot.
    int currentSlot_ = 0;

    Texture2D backgroundTexture_;
    Vector2 backgroundPosition_;

    Texture2D plankTexture_;
    Vector2 plankPosition_;

    Texture2D backTexture_;
    Vector2 backPosition_;

    Texture2D deleteTexture_;
    Vector2 deletePosition_{400.0f, 610.0f};
    Vector2 deleteTextPosition_{410.0f, 615.0f};

    Texture2D selectTexture_;
    Vector2 selectPosition_;

    Texture2D lineBorderTexture_;
    Vector2 lineBorderPosition_;

    Texture2D highlightTexture_;
    Texture2D arrowTexture_;

    Vector2 titleTextPosition_;
    Vector2 backTextPosition_;
    Vector2 selectTextPosition_;
};

} // namespace RolePlaying
