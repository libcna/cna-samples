#pragma once

// MainMenuScreen.hpp -- C++ port of MenuScreens/MainMenuScreen.cs.

#include <memory>
#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../AudioManager.hpp"
#include "../Combat/CombatEngine.hpp"
#include "../Data/GameStartDescription.hpp"
#include "../Fonts.hpp"
#include "../GameScreens/GameOverScreen.hpp"
#include "../GameScreens/GameplayScreen.hpp"
#include "../GameScreens/StatisticsScreen.hpp"
#include "../ScreenManager/MenuEntry.hpp"
#include "../ScreenManager/MenuScreen.hpp"
#include "../Session/SaveGameDescription.hpp"
#include "../Session/Session.hpp"
#include "ControlsScreen.hpp"
#include "HelpScreen.hpp"
#include "LoadingScreen.hpp"
#include "MessageBoxScreen.hpp"
#include "SaveLoadScreen.hpp"

namespace RolePlaying {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// The main menu screen is the first thing displayed when the game starts up.
class MainMenuScreen : public MenuScreen {
public:
    // Constructor fills in the menu contents.
    MainMenuScreen() {
        // add the New Game entry
        newGameMenuEntry_ = std::make_shared<MenuEntry>("New Game");
        newGameMenuEntry_->Description = "Start a New Game";
        newGameMenuEntry_->Font = &Fonts::HeaderFont();
        newGameMenuEntry_->Position = Vector2(715.0f, 0.0f);
        newGameMenuEntry_->Selected += [this](System::Object*, const System::EventArgs&) {
            NewGameMenuEntrySelected();
        };
        MenuEntries().push_back(newGameMenuEntry_);

        // add the Save Game menu entry, if the game has started but is not in combat
        if (Session::IsActive() && !CombatEngine::IsActive()) {
            saveGameMenuEntry_ = std::make_shared<MenuEntry>("Save Game");
            saveGameMenuEntry_->Description = "Save the Game";
            saveGameMenuEntry_->Font = &Fonts::HeaderFont();
            saveGameMenuEntry_->Position = Vector2(730.0f, 0.0f);
            saveGameMenuEntry_->Selected += [this](System::Object*, const System::EventArgs&) {
                SaveGameMenuEntrySelected();
            };
            MenuEntries().push_back(saveGameMenuEntry_);
        } else {
            saveGameMenuEntry_ = nullptr;
        }

        // add the Load Game menu entry
        loadGameMenuEntry_ = std::make_shared<MenuEntry>("Load Game");
        loadGameMenuEntry_->Description = "Load the Game";
        loadGameMenuEntry_->Font = &Fonts::HeaderFont();
        loadGameMenuEntry_->Position = Vector2(700.0f, 0.0f);
        loadGameMenuEntry_->Selected += [this](System::Object*, const System::EventArgs&) {
            LoadGameMenuEntrySelected();
        };
        MenuEntries().push_back(loadGameMenuEntry_);

        // add the Controls menu entry
        controlsMenuEntry_ = std::make_shared<MenuEntry>("Controls");
        controlsMenuEntry_->Description = "View Game Controls";
        controlsMenuEntry_->Font = &Fonts::HeaderFont();
        controlsMenuEntry_->Position = Vector2(720.0f, 0.0f);
        controlsMenuEntry_->Selected += [this](System::Object*, const System::EventArgs&) {
            GetScreenManager()->AddScreen(std::make_shared<ControlsScreen>());
        };
        MenuEntries().push_back(controlsMenuEntry_);

        // add the Help menu entry
        helpMenuEntry_ = std::make_shared<MenuEntry>("Help");
        helpMenuEntry_->Description = "View Game Help";
        helpMenuEntry_->Font = &Fonts::HeaderFont();
        helpMenuEntry_->Position = Vector2(700.0f, 0.0f);
        helpMenuEntry_->Selected += [this](System::Object*, const System::EventArgs&) {
            GetScreenManager()->AddScreen(std::make_shared<HelpScreen>());
        };
        MenuEntries().push_back(helpMenuEntry_);

        // create the Exit menu entry
        exitGameMenuEntry_ = std::make_shared<MenuEntry>("Exit");
        exitGameMenuEntry_->Description = "Quit the Game";
        exitGameMenuEntry_->Font = &Fonts::HeaderFont();
        exitGameMenuEntry_->Position = Vector2(720.0f, 0.0f);
        exitGameMenuEntry_->Selected += [this](System::Object*, const System::EventArgs&) { OnCancel(); };
        MenuEntries().push_back(exitGameMenuEntry_);

        // start the menu music
        AudioManager::PushMusic("MainTheme");
    }

    void LoadContent() override {
        // load the textures
        auto& content = GetScreenManager()->getGameProperty().getContentProperty();
        backgroundTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenu");
        descriptionAreaTexture_ = content.Load<Texture2D>("Textures/MainMenu/MainMenuInfoSpace");
        iconTexture_ = content.Load<Texture2D>("Textures/MainMenu/GameLogo");
        plankTexture1_ =
            std::make_shared<Texture2D>(content.Load<Texture2D>("Textures/MainMenu/MainMenuPlank"));
        plankTexture2_ = std::make_shared<Texture2D>(
            content.Load<Texture2D>("Textures/MainMenu/MainMenuPlank02"));
        plankTexture3_ = std::make_shared<Texture2D>(
            content.Load<Texture2D>("Textures/MainMenu/MainMenuPlank03"));
        backTexture_ = content.Load<Texture2D>("Textures/Buttons/BButton");
        selectTexture_ = content.Load<Texture2D>("Textures/Buttons/AButton");

        // calculate the texture positions
        auto viewport = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty();
        backgroundPosition_ = Vector2(
            (float)((viewport.getWidthProperty() - backgroundTexture_.getWidthProperty()) / 2),
            (float)((viewport.getHeightProperty() - backgroundTexture_.getHeightProperty()) / 2));
        descriptionAreaPosition_ = backgroundPosition_ + Vector2(158.0f, 130.0f);
        descriptionAreaTextPosition_ = backgroundPosition_ + Vector2(158.0f, 350.0f);
        iconPosition_ = backgroundPosition_ + Vector2(170.0f, 80.0f);
        backPosition_ = backgroundPosition_ + Vector2(225.0f, 610.0f);
        selectPosition_ = backgroundPosition_ + Vector2(1120.0f, 610.0f);

        // set the textures on each menu entry
        newGameMenuEntry_->Texture = plankTexture3_;
        if (saveGameMenuEntry_ != nullptr) {
            saveGameMenuEntry_->Texture = plankTexture2_;
        }
        loadGameMenuEntry_->Texture = plankTexture1_;
        controlsMenuEntry_->Texture = plankTexture2_;
        helpMenuEntry_->Texture = plankTexture3_;
        exitGameMenuEntry_->Texture = plankTexture1_;

        // now that they have textures, set the proper positions on the menu entries
        for (int i = 0; i < (int)MenuEntries().size(); i++) {
            MenuEntries()[(std::size_t)i]->Position = Vector2(
                MenuEntries()[(std::size_t)i]->Position.X,
                500.0f - (float)((MenuEntries()[(std::size_t)i]->Texture->getHeightProperty() - 10) *
                                 ((int)MenuEntries().size() - 1 - i)));
        }

        MenuScreen::LoadContent();
    }

    void HandleInput() override {
        if (InputManager::IsActionTriggered(InputManager::Action::Back) && Session::IsActive()) {
            AudioManager::PopMusic();
            ExitScreen();
            return;
        }

        MenuScreen::HandleInput();
    }

    void Draw(const GameTime& gameTime) override {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        spriteBatch.Begin();

        // draw the background images
        spriteBatch.Draw(backgroundTexture_, backgroundPosition_, Color::White);
        spriteBatch.Draw(descriptionAreaTexture_, descriptionAreaPosition_, Color::White);
        spriteBatch.Draw(iconTexture_, iconPosition_, Color::White);

        // Draw each menu entry in turn.
        for (int i = 0; i < (int)MenuEntries().size(); i++) {
            MenuEntry& menuEntry = *MenuEntries()[(std::size_t)i];
            bool isSelected = IsActive() && (i == selectedEntry_);
            menuEntry.Draw(*this, isSelected, gameTime);
        }

        // draw the description text for the selected entry
        std::shared_ptr<MenuEntry> selectedMenuEntry = SelectedMenuEntry();
        if (selectedMenuEntry != nullptr && !selectedMenuEntry->Description.empty()) {
            Vector2 textSize =
                Fonts::DescriptionFont().MeasureString(selectedMenuEntry->Description);
            Vector2 textPosition =
                descriptionAreaTextPosition_ +
                Vector2(std::floor(((float)descriptionAreaTexture_.getWidthProperty() - textSize.X) /
                                   2.0f),
                        0.0f);
            spriteBatch.DrawString(Fonts::DescriptionFont(), selectedMenuEntry->Description,
                                   textPosition, Color::White);
        }

        // draw the select instruction
        spriteBatch.Draw(selectTexture_, selectPosition_, Color::White);
        spriteBatch.DrawString(
            Fonts::ButtonNamesFont(), "Select",
            Vector2(selectPosition_.X - Fonts::ButtonNamesFont().MeasureString("Select").X - 5.0f,
                    selectPosition_.Y + 5.0f),
            Color::White);

        // if we are in-game, draw the back instruction
        if (Session::IsActive()) {
            spriteBatch.Draw(backTexture_, backPosition_, Color::White);
            spriteBatch.DrawString(Fonts::ButtonNamesFont(), "Resume",
                                   Vector2(backPosition_.X + 55.0f, backPosition_.Y + 5.0f),
                                   Color::White);
        }

        spriteBatch.End();
    }

protected:
    // When the user cancels the main menu, or when the Exit Game menu entry is selected.
    void OnCancel() override {
        // add a confirmation message box
        std::string message;
        if (Session::IsActive()) {
            message = "Are you sure you want to exit?  All unsaved progress will be lost.";
        } else {
            message = "Are you sure you want to exit?";
        }
        auto confirmExitMessageBox = std::make_shared<MessageBoxScreen>(message);
        confirmExitMessageBox->Accepted += [this](System::Object*, const System::EventArgs&) {
            GetScreenManager()->getGameProperty().Exit();
        };
        GetScreenManager()->AddScreen(confirmExitMessageBox);
    }

private:
    // Event handler for when the New Game menu entry is selected.
    void NewGameMenuEntrySelected() {
        if (Session::IsActive()) {
            ExitScreen();
        }

        auto& content = GetScreenManager()->getGameProperty().getContentProperty();
        std::vector<std::shared_ptr<GameScreen>> toLoad;
        toLoad.push_back(std::make_shared<GameplayScreen>(
            content.Load<std::shared_ptr<RolePlayingGameData::GameStartDescription>>(
                "MainGameDescription")));
        LoadingScreen::Load(*GetScreenManager(), true, toLoad);
    }

    // Event handler for when the Save Game menu entry is selected.
    void SaveGameMenuEntrySelected() {
        GetScreenManager()->AddScreen(
            std::make_shared<SaveLoadScreen>(SaveLoadScreen::SaveLoadScreenMode::Save));
    }

    // Event handler for when the Load Game menu entry is selected.
    void LoadGameMenuEntrySelected() {
        auto loadGameScreen =
            std::make_shared<SaveLoadScreen>(SaveLoadScreen::SaveLoadScreenMode::Load);
        loadGameScreen->LoadingSaveGame = [this](const SaveGameDescription& saveGameDescription) {
            LoadGameScreenLoadingSaveGame(saveGameDescription);
        };
        GetScreenManager()->AddScreen(loadGameScreen);
    }

    // Handle save-game-to-load-selected events from the SaveLoadScreen.
    void LoadGameScreenLoadingSaveGame(const SaveGameDescription& saveGameDescription) {
        if (Session::IsActive()) {
            ExitScreen();
        }
        std::vector<std::shared_ptr<GameScreen>> toLoad;
        toLoad.push_back(std::make_shared<GameplayScreen>(saveGameDescription));
        LoadingScreen::Load(*GetScreenManager(), true, toLoad);
    }

    Texture2D backgroundTexture_;
    Vector2 backgroundPosition_;

    Texture2D descriptionAreaTexture_;
    Vector2 descriptionAreaPosition_;
    Vector2 descriptionAreaTextPosition_;

    Texture2D iconTexture_;
    Vector2 iconPosition_;

    Texture2D backTexture_;
    Vector2 backPosition_;

    Texture2D selectTexture_;
    Vector2 selectPosition_;

    std::shared_ptr<Texture2D> plankTexture1_, plankTexture2_, plankTexture3_;

    std::shared_ptr<MenuEntry> newGameMenuEntry_, exitGameMenuEntry_;
    std::shared_ptr<MenuEntry> saveGameMenuEntry_, loadGameMenuEntry_;
    std::shared_ptr<MenuEntry> controlsMenuEntry_, helpMenuEntry_;
};

// ---- screens that open the main menu (defined here) ----

inline void GameOverScreen::HandleInput() {
    if (InputManager::IsActionTriggered(InputManager::Action::Ok) ||
        InputManager::IsActionTriggered(InputManager::Action::Back)) {
        ExitScreen();
        GetScreenManager()->AddScreen(std::make_shared<MainMenuScreen>());
        return;
    }
}

// ---- GameplayScreen methods that depend on MainMenuScreen (defined here) ----

inline void GameplayScreen::HandleInput() {
    if (InputManager::IsActionTriggered(InputManager::Action::MainMenu)) {
        GetScreenManager()->AddScreen(std::make_shared<MainMenuScreen>());
        return;
    }

    if (InputManager::IsActionTriggered(InputManager::Action::ExitGame)) {
        // add a confirmation message box
        const std::string message =
            "Are you sure you want to exit?  All unsaved progress will be lost.";
        auto confirmExitMessageBox = std::make_shared<MessageBoxScreen>(message);
        confirmExitMessageBox->Accepted += [this](System::Object*, const System::EventArgs&) {
            GetScreenManager()->getGameProperty().Exit();
        };
        GetScreenManager()->AddScreen(confirmExitMessageBox);
        return;
    }

    if (!CombatEngine::IsActive() &&
        InputManager::IsActionTriggered(InputManager::Action::CharacterManagement)) {
        GetScreenManager()->AddScreen(
            std::make_shared<StatisticsScreen>(Session::GetParty()->Players[0]));
        return;
    }
}

} // namespace RolePlaying
