#pragma once

// GameplayScreenBodies.hpp -- the gameplay screen's bodies.
//
// They need MainMenuScreen and SelectOnlineGameScreen, which the screen pushes when the game
// ends, and the game's fonts. Included from the end of YachtGame.hpp, once everything is whole.

#include "../YachtGame.hpp"
#include "GameplayScreen.hpp"
#include "MainMenuScreen.hpp"
#include "SelectOnlineGameScreen.hpp"

namespace Yacht {

inline std::shared_ptr<YachtState> GameplayScreen::CurrentYachtState() const
{
    auto& state =
        Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty().getStateProperty();
    std::shared_ptr<System::Object> stored;
    if (!state.TryGetValue(Constants::YachtStateKey, stored)) {
        return nullptr;
    }
    return std::dynamic_pointer_cast<YachtState>(stored);
}

inline void GameplayScreen::InitializeDiceHandler(const std::shared_ptr<DiceState>& diceState)
{
    diceHandler_ = std::make_unique<DiceHandler>(
        getScreenManagerProperty()->getGameProperty().getGraphicsDeviceProperty(), diceState);
    diceHandler_->LoadAssets(getScreenManagerProperty()->getGameProperty().getContentProperty());
}

inline void GameplayScreen::InitializeGameStateHandler(
    const std::shared_ptr<YachtServices::GameState>& state)
{
    gameStateHandler_ = std::make_unique<GameStateHandler>(
        *diceHandler_, getScreenManagerProperty()->input, name_, state,
        getScreenManagerProperty()->getGameProperty().getGraphicsDeviceProperty()
            .getViewportProperty().getBoundsProperty(),
        getScreenManagerProperty()->getGameProperty().getContentProperty());
}

inline void GameplayScreen::LoadContent()
{
    GameScreen::LoadContent();

    Dice::LoadAssets(getScreenManagerProperty()->getGameProperty().getContentProperty());
    background_.emplace(Load<Texture2D>("Images/bg"));

    // When reaching a gameplay screen, we know that there is a yacht state in the current
    // state object.
    auto yachtState = CurrentYachtState();
    if (yachtState == nullptr) {
        yachtState = std::make_shared<YachtState>();
        Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty()
            .getStateProperty()
            .Add(Constants::YachtStateKey, yachtState);
    }

    InitializeDiceHandler(yachtState->PlayerDiceState);
    yachtState->PlayerDiceState = diceHandler_->getDiceStatePointerEXT();
    diceHandler_->PositionDice();

    if (gameType_ == YachtServices::GameTypes::Offline) {
        InitializeGameStateHandler(yachtState->YachGameState);
        yachtState->YachGameState = gameStateHandler_->getStatePointerEXT();
        return;
    }

    auto* network = NetworkManager::getInstanceProperty();
    if (network == nullptr) {
        return;
    }

    // Register for network notifications
    gameStateArrivedToken_ = network->GameStateArrived.Add(
        [this](System::Object*, const YachtGameStateEventArgs& e) { ServerGameStateArrived(e); });
    gameOverToken_ = network->GameOver.Add(
        [this](System::Object*, const YachtGameOverEventArgs& e) { GameOverEnded(e); });
    bannedToken_ =
        network->Banned.Add([this](System::Object*, const System::EventArgs&) { Banned(); });
    gameUnavailableToken_ = network->GameUnavailable.Add(
        [this](System::Object*, const System::EventArgs&) { GameUnavailable(); });
    serviceErrorToken_ = network->ServiceError.Add(
        [this](System::Object*, const ExceptionEventArgs& e) { ServerErrorOccurred(e); });

    network->GetGameState();
}

inline void GameplayScreen::Dispose()
{
    auto* network = NetworkManager::getInstanceProperty();
    if (network == nullptr) {
        return;
    }

    network->GameStateArrived.Remove(gameStateArrivedToken_);
    network->ServiceError.Remove(serviceErrorToken_);
    network->Banned.Remove(bannedToken_);
    network->GameUnavailable.Remove(gameUnavailableToken_);
    network->ScoreCardArrived.Remove(scoreCardArrivedToken_);
    network->GameOver.Remove(gameOverToken_);
}

inline void GameplayScreen::GameOverEnded(const YachtGameOverEventArgs& e)
{
    if (gameStateHandler_ == nullptr || e.EndGameState == nullptr) {
        return;
    }

    gameStateHandler_->ShowGameOver(*e.EndGameState);

    if (dynamic_cast<HumanPlayer*>(gameStateHandler_->WinnerPlayer()) != nullptr) {
        AudioManager::PlaySound("Winner");
    } else {
        AudioManager::PlaySound("Loss");
    }
}

inline void GameplayScreen::HandleInput(InputState& input)
{
    if (!Guide::getIsVisibleProperty()) {
        if (input.IsPauseGame(std::nullopt)) {
            QuiteGame();
        }

        if (gameStateHandler_ != nullptr && gameStateHandler_->IsGameOver() &&
            !input.Gestures.empty() &&
            input.Gestures[0].getGestureTypeProperty() == GestureType::Tap) {
            Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty()
                .getStateProperty()
                .Remove(Constants::YachtStateKey);
            ExitScreen();
            Dispose();

            if (gameStateHandler_->State().GameType == YachtServices::GameTypes::Offline) {
                getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(),
                                                      std::nullopt);
            } else {
                auto* network = NetworkManager::getInstanceProperty();
                getScreenManagerProperty()->AddScreen(
                    std::make_shared<SelectOnlineGameScreen>(
                        network != nullptr ? network->name : std::string()),
                    std::nullopt);
            }
        }

        for (const GestureSample& gesture : input.Gestures) {
            if (gameStateHandler_ != nullptr && gameStateHandler_->IsInitialized()) {
                gameStateHandler_->HandleInput(gesture);
            }
        }
    }

    GameScreen::HandleInput(input);
}

inline void GameplayScreen::QuiteGame()
{
    if (gameStateHandler_ == nullptr) {
        return;
    }

    if (gameStateHandler_->State().GameType == YachtServices::GameTypes::Offline) {
        (void)Guide::BeginShowMessageBox(
            "Save Game", "Do you want to save your progress?",
            std::vector<std::string>{"Yes", "No"}, 0, MessageBoxIcon::Warning,
            [this](System::IAsyncResult& result) { ShowSaveDialogEnded(result); }, {});
        return;
    }

    if (gameStateHandler_->IsGameOver()) {
        HandleExitScreen();
        return;
    }

    (void)Guide::BeginShowMessageBox(
        "Are you sure you want to leave the game?", " ", std::vector<std::string>{"Yes", "No"}, 0,
        MessageBoxIcon::Warning,
        [this](System::IAsyncResult& result) { AbortExitDialogEnded(result); }, {});
}

inline void GameplayScreen::AbortExitDialogEnded(System::IAsyncResult& result)
{
    const std::optional<int> res = Guide::EndShowMessageBox(&result);
    delete &result;

    if (res.has_value() && *res == 0) {
        HandleExitScreen();
    }
}

inline void GameplayScreen::HandleExitScreen()
{
    ExitScreen();
    Dispose();
    Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty()
        .getStateProperty()
        .Remove(Constants::YachtStateKey);

    if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
        network->Unregister();
    }

    getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
}

inline void GameplayScreen::ShowSaveDialogEnded(System::IAsyncResult& result)
{
    const std::optional<int> res = Guide::EndShowMessageBox(&result);
    delete &result;

    if (!res.has_value()) {
        return;
    }

    if (*res == 0) {
        YachtGame::SaveGameState();
    }

    Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty()
        .getStateProperty()
        .Remove(Constants::YachtStateKey);
    ExitScreen();
    Dispose();
    getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
}

inline void GameplayScreen::MakeAIPlay(AIPlayer* player)
{
    timer_.reset();
    player->PerformPlayerLogic();
}

inline void GameplayScreen::Update(GameTime& gameTime, bool otherScreenHasFocus,
                                   bool coveredByOtherScreen)
{
    if (!Guide::getIsVisibleProperty() && gameStateHandler_ != nullptr &&
        !gameStateHandler_->IsGameOver()) {
        auto* held = diceHandler_->GetHoldingDice();
        gameStateHandler_->setScoreDice(
            held == nullptr ? std::nullopt
                            : std::optional(GameStateHandler::ToRawDice(*held)));
        diceHandler_->Update();

        if (gameStateHandler_->IsInitialized() && gameStateHandler_->CurrentPlayer() != nullptr &&
            !gameStateHandler_->IsWaitingForPlayer()) {
            auto* current = gameStateHandler_->CurrentPlayer();

            if (dynamic_cast<AIPlayer*>(current) == nullptr) {
                current->PerformPlayerLogic();
            } else if (timer_ == nullptr) {
                // The computer opponents pause before moving, so a turn does not flash past.
                pendingAIPlayer_ = dynamic_cast<AIPlayer*>(current);
                timer_ = std::make_unique<System::Threading::Timer>(
                    [](void* state) {
                        auto* screen = static_cast<GameplayScreen*>(state);
                        if (screen->pendingAIPlayer_ != nullptr) {
                            screen->MakeAIPlay(screen->pendingAIPlayer_);
                        }
                    },
                    this, random_.Next(300, 600), -1);
            }
        }
    }

    GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
}

inline void GameplayScreen::Draw(const GameTime& gameTime)
{
    auto* screenManager = getScreenManagerProperty();
    screenManager->getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);
    SpriteBatch& spriteBatch = screenManager->getSpriteBatchProperty();

    spriteBatch.Begin();
    spriteBatch.Draw(*background_, Vector2::Zero, Color::White);

    if (!Guide::getIsVisibleProperty()) {
        if (gameStateHandler_ != nullptr && gameStateHandler_->IsInitialized()) {
            if (diceHandler_ != nullptr && !gameStateHandler_->IsGameOver()) {
                diceHandler_->Draw(spriteBatch);

                if (!gameStateHandler_->IsWaitingForPlayer()) {
                    gameStateHandler_->CurrentPlayer()->Draw(spriteBatch);
                }
            }

            gameStateHandler_->Draw(spriteBatch);
        }

        DrawGameOver();
    }

    spriteBatch.End();

    GameScreen::Draw(gameTime);
}

inline void GameplayScreen::DrawGameOver()
{
    if (gameStateHandler_ == nullptr || !gameStateHandler_->IsGameOver()) {
        return;
    }

    auto* screenManager = getScreenManagerProperty();
    const Rectangle screenBounds = screenManager->getGameProperty()
                                       .getGraphicsDeviceProperty()
                                       .getViewportProperty()
                                       .getBoundsProperty();
    const std::string winnerText =
        gameStateHandler_->WinnerPlayer()->getNameProperty() + " is the winner!";
    const Vector2 measure = YachtGame::Font->MeasureString(winnerText);
    const Vector2 position(static_cast<float>(screenBounds.getCenterProperty().X) - measure.X / 2,
                           static_cast<float>(screenBounds.getBottomProperty() - 100));

    screenManager->getSpriteBatchProperty().DrawString(*YachtGame::Font, winnerText, position,
                                                       Color::White);
}

inline void GameplayScreen::GameUnavailable()
{
    auto* network = NetworkManager::getInstanceProperty();
    const std::string gameName = network != nullptr ? network->gameName : std::string();

    (void)Guide::BeginShowMessageBox(
        "The game " + gameName + " is unavailable",
        "Do you want to create a new game with same name, or join another game?",
        std::vector<std::string>{"Create", "Join"}, 0, MessageBoxIcon::None,
        [this](System::IAsyncResult& result) { UnavailableGameDialogEnded(result); }, {});
}

inline void GameplayScreen::UnavailableGameDialogEnded(System::IAsyncResult& result)
{
    const std::optional<int> res = Guide::EndShowMessageBox(&result);
    delete &result;

    auto* network = NetworkManager::getInstanceProperty();

    if (res.has_value() && *res == 0) {
        if (network != nullptr) {
            newGameCreatedToken_ = network->NewGameCreated.Add(
                [this](System::Object*, const BooleanEventArgs& e) { NewGameCreated(e); });
            network->NewGame(network->gameName);
        }
        return;
    }

    Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty()
        .getStateProperty()
        .Remove(Constants::YachtStateKey);
    ExitScreen();
    Dispose();

    if (res.has_value() && *res == 1) {
        getScreenManagerProperty()->AddScreen(
            std::make_shared<SelectOnlineGameScreen>(network != nullptr ? network->name
                                                                       : std::string()),
            std::nullopt);
    } else {
        getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    }
}

inline void GameplayScreen::NewGameCreated(const BooleanEventArgs& e)
{
    auto* network = NetworkManager::getInstanceProperty();
    if (network != nullptr) {
        network->NewGameCreated.Remove(newGameCreatedToken_);
    }

    if (e.Answer) {
        if (network != nullptr) {
            network->GetGameState();
        }
        return;
    }

    (void)Guide::BeginShowMessageBox("Cannot create the game with same name the name is in use",
                                     "", std::vector<std::string>{"OK"}, 0, MessageBoxIcon::Alert,
                                     nullptr, {});
    ExitScreen();
    Dispose();
    getScreenManagerProperty()->AddScreen(
        std::make_shared<SelectOnlineGameScreen>(network != nullptr ? network->name
                                                                    : std::string()),
        std::nullopt);
}

inline void GameplayScreen::ServerErrorOccurred(const ExceptionEventArgs&)
{
    (void)Guide::BeginShowMessageBox(
        "There was a server error. Please try to connect again.", " ",
        std::vector<std::string>{"OK"}, 0, MessageBoxIcon::Error,
        [this](System::IAsyncResult& result) { ErrorDialogEnded(result); }, {});
}

inline void GameplayScreen::ErrorDialogEnded(System::IAsyncResult& result)
{
    delete &result;

    auto* network = NetworkManager::getInstanceProperty();
    ExitScreen();
    Dispose();
    getScreenManagerProperty()->AddScreen(
        std::make_shared<SelectOnlineGameScreen>(network != nullptr ? network->name
                                                                    : std::string()),
        std::nullopt);
}

inline void GameplayScreen::Banned()
{
    ExitScreen();
    Dispose();
    getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
}

inline void GameplayScreen::ServerGameStateArrived(const YachtGameStateEventArgs& e)
{
    if (e.GameState == nullptr) {
        return;
    }

    auto* network = NetworkManager::getInstanceProperty();

    if (gameStateHandler_ == nullptr) {
        InitializeGameStateHandler(e.GameState);

        if (auto yachtState = CurrentYachtState(); yachtState != nullptr) {
            yachtState->NetworkManagerState = network;
            yachtState->YachGameState = gameStateHandler_->getStatePointerEXT();
        }

        if (diceHandler_->getDiceStateProperty().ValidForTurn != e.GameState->StepsMade) {
            diceHandler_->Reset(false);
        }

        if (network != nullptr && !subscribedToScoreCard_) {
            scoreCardArrivedToken_ = network->ScoreCardArrived.Add(
                [this](System::Object*, const YachtScoreCardEventArgs& args) {
                    ServerScoreCardArrived(args);
                });
            subscribedToScoreCard_ = true;
        }
    } else {
        if (e.GameState->StepsMade != gameStateHandler_->State().StepsMade) {
            diceHandler_->Reset(false);
        }
        gameStateHandler_->SetState(*e.GameState);
    }

    diceHandler_->getDiceStatePointerEXT()->ValidForTurn = gameStateHandler_->State().StepsMade;
    AudioManager::PlaySoundRandom("TurnChange", 2);

    const auto currentPlayer = static_cast<std::size_t>(e.GameState->CurrentPlayer);
    if (network != nullptr && currentPlayer < e.GameState->Players.size() &&
        e.GameState->Players[currentPlayer].PlayerID == network->playerID) {
        network->GetScoreCard();
    }
}

inline void GameplayScreen::ServerScoreCardArrived(const YachtScoreCardEventArgs& e)
{
    if (gameStateHandler_ != nullptr) {
        gameStateHandler_->UpdateScoreCard(e.ScoreCard);
    }
}

} // namespace Yacht
