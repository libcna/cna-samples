#pragma once

// GameplayScreen.hpp -- C++ port of GameScreens/GameplayScreen.cs.

#include <memory>
#include <optional>

#include "../Combat/CombatEngine.hpp"
#include "../Data/GameStartDescription.hpp"
#include "../InputManager.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "../Session/SaveGameDescription.hpp"
#include "../Session/Session.hpp"

namespace RolePlaying {

// This screen implements the actual game logic.
class GameplayScreen : public GameScreen {
public:
    // Create a new GameplayScreen object from a new-game description.
    explicit GameplayScreen(
        std::shared_ptr<RolePlayingGameData::GameStartDescription> gameStartDescription)
        : GameplayScreen() {
        gameStartDescription_ = std::move(gameStartDescription);
        saveGameDescription_.reset();
    }

    // Create a new GameplayScreen object from a saved-game description.
    explicit GameplayScreen(const SaveGameDescription& saveGameDescription) : GameplayScreen() {
        gameStartDescription_ = nullptr;
        saveGameDescription_ = saveGameDescription;
    }

    void LoadContent() override {
        if (gameStartDescription_ != nullptr) {
            Session::StartNewSession(*gameStartDescription_, *GetScreenManager(), *this);
        } else if (saveGameDescription_) {
            Session::LoadSession(*saveGameDescription_, *GetScreenManager(), *this);
        }

        // once the load has finished, we use ResetElapsedTime to tell the game's timing
        // mechanism that we have just finished a very long frame, and that it should not try to
        // catch up.
        GetScreenManager()->getGameProperty().ResetElapsedTime();
    }

    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override {
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

        if (IsActive() && !coveredByOtherScreen) {
            Session::Update(gameTime);
        }
    }

    // Defined out-of-line in MainMenuScreen.hpp -- it opens screens that in turn need this one.
    void HandleInput() override;

    void Draw(const GameTime& gameTime) override { Session::Draw(gameTime); }

private:
    GameplayScreen() {
        CombatEngine::ClearCombat();
        // Mirrors the original's Exiting event hookup -- EndSession must be re-entrant safe,
        // since EndSession may itself be closing this screen.
        Exiting = [] { Session::EndSession(); };
    }

    std::shared_ptr<RolePlayingGameData::GameStartDescription> gameStartDescription_;
    std::optional<SaveGameDescription> saveGameDescription_;
};

} // namespace RolePlaying
