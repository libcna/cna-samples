#pragma once

// GameplayScreen.hpp -- C++ port of Yacht/Screens/GameplayScreen.cs.

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Phone/Shell/PhoneApplicationService.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/MessageBoxIcon.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Random.hpp"
#include "System/Threading/Timer.hpp"

#include "../Constants.hpp"
#include "../GameStateHandler.hpp"
#include "../Misc/NetworkManager.hpp"
#include "../Objects/DiceHandler.hpp"
#include "../ScreenManager/ScreenBodies.hpp"
#include "../YachtState.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::GamerServices::Guide;
using Microsoft::Xna::Framework::GamerServices::MessageBoxIcon;

class YachtGame;

/**
 * @brief The board: the score card, the leaderboard, the dice and whoever is playing them.
 *
 * The same screen serves both kinds of game. Offline it owns the state and moves the turn on
 * itself; online it owns nothing -- the server sends a state, the screen adopts it, and every
 * move goes back over the wire.
 */
class GameplayScreen : public GameScreen {
public:
    /** @brief The type's name. @return "GameplayScreen". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "GameplayScreen";
        return name;
    }

    /**
     * @brief Initialize a new game screen.
     *
     * @param gameType The type of game for which this screen is created.
     */
    explicit GameplayScreen(YachtServices::GameTypes gameType) : gameType_(gameType)
    {
        setEnabledGesturesProperty(GestureType::Tap | GestureType::VerticalDrag |
                                   GestureType::DragComplete);
    }

    /**
     * @brief Initialize a new game screen.
     *
     * @param name     The name of the human player participating in the game.
     * @param gameType The type of game for which this screen is created.
     */
    GameplayScreen(std::string name, YachtServices::GameTypes gameType) : GameplayScreen(gameType)
    {
        name_ = std::move(name);
    }

    /** @brief Unsubscribes from the server's events. */
    void Dispose();

    /** @brief Load graphics content for the game. */
    void LoadContent() override;

    /**
     * @brief Handles the player's input.
     *
     * @param input This frame's input.
     */
    void HandleInput(InputState& input) override;

    /**
     * @brief Runs the game's logic.
     *
     * @param gameTime             The elapsed time.
     * @param otherScreenHasFocus  Whether another screen has focus.
     * @param coveredByOtherScreen Whether another screen covers this one.
     */
    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override;

    /**
     * @brief Draws the board.
     *
     * @param gameTime The elapsed time.
     */
    void Draw(const GameTime& gameTime) override;

private:
    void InitializeDiceHandler(const std::shared_ptr<DiceState>& diceState);
    void InitializeGameStateHandler(const std::shared_ptr<YachtServices::GameState>& state);
    void QuiteGame();
    void HandleExitScreen();
    void AbortExitDialogEnded(System::IAsyncResult& result);
    void ShowSaveDialogEnded(System::IAsyncResult& result);
    void UnavailableGameDialogEnded(System::IAsyncResult& result);
    void ErrorDialogEnded(System::IAsyncResult& result);
    void DrawGameOver();
    void GameOverEnded(const YachtGameOverEventArgs& e);
    void GameUnavailable();
    void NewGameCreated(const BooleanEventArgs& e);
    void ServerErrorOccurred(const ExceptionEventArgs& e);
    void Banned();
    void ServerGameStateArrived(const YachtGameStateEventArgs& e);
    void ServerScoreCardArrived(const YachtScoreCardEventArgs& e);
    void MakeAIPlay(AIPlayer* player);

    // The screen's own state object, so a saved game and this screen are looking at the same
    // one -- see YachtState.
    [[nodiscard]] std::shared_ptr<YachtState> CurrentYachtState() const;

    std::optional<Texture2D> background_;
    std::unique_ptr<GameStateHandler> gameStateHandler_;
    std::unique_ptr<DiceHandler> diceHandler_;
    std::string name_;
    std::unique_ptr<System::Threading::Timer> timer_;
    System::Random random_;

    // Which computer player the pending timer will move. The original passes the player as the
    // timer's state; here the timer carries the screen and reads it back from this, which keeps
    // the callback free of anything that could outlive the turn.
    AIPlayer* pendingAIPlayer_ = nullptr;
    YachtServices::GameTypes gameType_ = YachtServices::GameTypes::Offline;

    // Subscriptions to the network manager, so Dispose can take them back off again.
    System::EventHandler<YachtGameStateEventArgs>::Token gameStateArrivedToken_ = 0;
    System::EventHandler<ExceptionEventArgs>::Token serviceErrorToken_ = 0;
    System::EventHandler<System::EventArgs>::Token bannedToken_ = 0;
    System::EventHandler<System::EventArgs>::Token gameUnavailableToken_ = 0;
    System::EventHandler<YachtScoreCardEventArgs>::Token scoreCardArrivedToken_ = 0;
    System::EventHandler<YachtGameOverEventArgs>::Token gameOverToken_ = 0;
    System::EventHandler<BooleanEventArgs>::Token newGameCreatedToken_ = 0;
    bool subscribedToScoreCard_ = false;
};

} // namespace Yacht
