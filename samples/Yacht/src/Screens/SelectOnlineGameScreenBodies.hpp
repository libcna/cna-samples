#pragma once

// SelectOnlineGameScreenBodies.hpp -- the lobby's bodies.
//
// It draws with the game's font and pushes the gameplay and main-menu screens, so it closes
// here with the rest. Included from the end of YachtGame.hpp.

#include "../YachtGame.hpp"
#include "GameplayScreen.hpp"
#include "MainMenuScreen.hpp"
#include "SelectOnlineGameScreen.hpp"

namespace Yacht {

inline SelectOnlineGameScreen::SelectOnlineGameScreen(std::string name) : name_(std::move(name))
{
    if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
        registeredToken_ = network->Registered.Add(
            [this](System::Object*, const BooleanEventArgs& e) { InstanceRegistered(e); });
        serviceErrorToken_ = network->ServiceError.Add(
            [this](System::Object*, const ExceptionEventArgs&) { InstanceError(); });
        availableGamesToken_ = network->AvailableGamesArrived.Add(
            [this](System::Object*, const YachtAvailableGamesEventArgs& e) {
                InstanceAvailableGamesArrived(e);
            });
        newGameCreatedToken_ = network->NewGameCreated.Add(
            [this](System::Object*, const BooleanEventArgs& e) { InstanceNewGameCreated(e); });
        joinedGameToken_ = network->JoinedGame.Add(
            [this](System::Object*, const BooleanEventArgs& e) { InstanceJoinedGame(e); });

        network->Connect(name_);
    }

    setEnabledGesturesProperty(GestureType::Tap | GestureType::VerticalDrag |
                               GestureType::DragComplete);
}

inline void SelectOnlineGameScreen::Dispose()
{
    auto* network = NetworkManager::getInstanceProperty();
    if (network == nullptr) {
        return;
    }

    network->Registered.Remove(registeredToken_);
    network->ServiceError.Remove(serviceErrorToken_);
    network->AvailableGamesArrived.Remove(availableGamesToken_);
    network->NewGameCreated.Remove(newGameCreatedToken_);
    network->JoinedGame.Remove(joinedGameToken_);
}

inline void SelectOnlineGameScreen::LoadContent()
{
    auto& content = getScreenManagerProperty()->getGameProperty().getContentProperty();

    background_.emplace(content.template Load<Texture2D>("Images/online_game_selectionBG"));
    line_.emplace(content.template Load<Texture2D>("Images/button"));
    scrollThumb_.emplace(content.template Load<Texture2D>("Images/ScrollThumb"));
    buttonTexture_.emplace(content.template Load<Texture2D>("Images/button"));

    connect_.emplace(&*buttonTexture_, Rectangle(5, 700, 140, 60), YachtGame::Font, "Connect");
    connect_->Click += [this](System::Object*, const System::EventArgs&) { ConnectClick(); };

    searchAgain_.emplace(&*buttonTexture_, Rectangle(170, 700, 140, 60), YachtGame::Font,
                         "Search");
    searchAgain_->Click += [this](System::Object*, const System::EventArgs&) {
        SearchAgainClick();
    };

    newGame_.emplace(&*buttonTexture_, Rectangle(340, 700, 140, 60), YachtGame::Font, "New Game");
    newGame_->Click += [this](System::Object*, const System::EventArgs&) { NewGameClick(); };

    GameScreen::LoadContent();
}

inline void SelectOnlineGameScreen::HandleInput(InputState& input)
{
    if (input.IsPauseGame(std::nullopt)) {
        ExitScreen();
        Dispose();
        if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
            network->Unregister();
        }
        getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    }

    for (const GestureSample& gesture : input.Gestures) {
        connect_->HandleInput(gesture);
        searchAgain_->HandleInput(gesture);
        newGame_->HandleInput(gesture);
        HandleDragList(gesture);
        HandleSelectedGameInput(gesture);
    }

    GameScreen::HandleInput(input);
}

inline void SelectOnlineGameScreen::HandleDragList(const GestureSample& sample)
{
    if (availableGames_ == nullptr ||
        sample.getGestureTypeProperty() != GestureType::VerticalDrag) {
        return;
    }

    const Rectangle bounds(10, 100, line_->getWidthProperty(),
                           line_->getHeightProperty() * maxShowGames_);
    const Rectangle touchRect(static_cast<int>(sample.getPositionProperty().X) - 5,
                              static_cast<int>(sample.getPositionProperty().Y) - 5, 10, 10);

    if (bounds.Intersects(touchRect)) {
        offset_.Y -= sample.getDeltaProperty().Y;
        offset_.Y = MathHelper::Clamp(
            offset_.Y, 0.0f,
            static_cast<float>(line_->getHeightProperty() *
                               (static_cast<int>(availableGames_->Games.size()) - maxShowGames_)));
    }
}

inline void SelectOnlineGameScreen::HandleSelectedGameInput(const GestureSample& sample)
{
    if (availableGames_ == nullptr || sample.getGestureTypeProperty() != GestureType::Tap) {
        return;
    }

    const int heightInteval = 60;
    const Rectangle screenBounds = getScreenManagerProperty()
                                       ->getGraphicsDeviceProperty()
                                       .getViewportProperty()
                                       .getBoundsProperty();
    const Rectangle touchRect(static_cast<int>(sample.getPositionProperty().X) - 5,
                              static_cast<int>(sample.getPositionProperty().Y) - 5, 10, 10);

    const int from = static_cast<int>(offset_.Y) / heightInteval;
    const int to = static_cast<int>(MathHelper::Clamp(
        static_cast<float>(from + maxShowGames_), 0.0f,
        static_cast<float>(availableGames_->Games.size())));

    for (int i = from; i < to; i++) {
        const Rectangle gameRect(0, 200 + (i - from) * heightInteval,
                                 screenBounds.Width, line_->getHeightProperty());
        if (gameRect.Intersects(touchRect)) {
            selectedLine_ = i;
        }
    }
}

inline void SelectOnlineGameScreen::Update(GameTime& gameTime, bool otherScreenHasFocus,
                                           bool coveredByOtherScreen)
{
    connect_->Enabled = isConnected_ && selectedLine_.has_value();
    searchAgain_->Enabled = !isConnecting_ && !isSearching_;
    newGame_->Enabled = isConnected_;

    GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
}

inline void SelectOnlineGameScreen::Draw(const GameTime& gameTime)
{
    SpriteBatch& spriteBatch = getScreenManagerProperty()->getSpriteBatchProperty();

    spriteBatch.Begin();
    spriteBatch.Draw(*background_, Vector2::Zero, Color::White);
    searchAgain_->Draw(spriteBatch);
    newGame_->Draw(spriteBatch);
    connect_->Draw(spriteBatch);

    if (isConnecting_) {
        // "Connecting..." types itself out one character every thirty frames, which is the
        // original's way of showing the wait without an animation.
        frame_++;
        const auto shown = static_cast<std::size_t>(frame_ / 30);
        spriteBatch.DrawString(*YachtGame::Font,
                               text_.substr(0, std::min(shown, text_.size())), Vector2(10, 10),
                               Color::White);
        if (shown == text_.size()) {
            frame_ = 300;
        }
    } else if (hasError_) {
        spriteBatch.DrawString(*YachtGame::Font, error_, Vector2(10, 10), Color::White);
    } else if (availableGames_ != nullptr) {
        DrawAvailableGames(spriteBatch);
    }

    spriteBatch.End();

    GameScreen::Draw(gameTime);
}

inline void SelectOnlineGameScreen::DrawAvailableGames(SpriteBatch& spriteBatch)
{
    const Rectangle screenBounds = getScreenManagerProperty()
                                       ->getGraphicsDeviceProperty()
                                       .getViewportProperty()
                                       .getBoundsProperty();
    const int heightInteval = 60;
    const int from = static_cast<int>(offset_.Y) / heightInteval;
    const int to = static_cast<int>(MathHelper::Clamp(
        static_cast<float>(from + maxShowGames_), 0.0f,
        static_cast<float>(availableGames_->Games.size())));

    for (int i = from; i < to; i++) {
        spriteBatch.Draw(*line_,
                         Rectangle(0, 200 + (i - from) * heightInteval, screenBounds.Width,
                                   line_->getHeightProperty()),
                         Color::White);

        const std::string& gameName = availableGames_->Games[static_cast<std::size_t>(i)].Name;
        const Vector2 sizeOfText = YachtGame::Font->MeasureString(gameName);
        spriteBatch.DrawString(
            *YachtGame::Font, gameName,
            Vector2(static_cast<float>(screenBounds.getCenterProperty().X) - sizeOfText.X / 2,
                    static_cast<float>(200 + (i - from) * heightInteval)),
            selectedLine_.has_value() && i == *selectedLine_ ? Color::Red : Color::White);
    }

    if (availableGames_->Games.empty()) {
        spriteBatch.DrawString(*YachtGame::Font, "No available games", Vector2(10, 220),
                               Color::White);
    } else if (static_cast<int>(availableGames_->Games.size()) > maxShowGames_) {
        const float scrollYPos =
            static_cast<float>(heightInteval * maxShowGames_ - scrollThumb_->getHeightProperty()) /
            static_cast<float>(heightInteval * static_cast<int>(availableGames_->Games.size()) -
                               heightInteval * maxShowGames_) *
            offset_.Y;
        spriteBatch.Draw(*scrollThumb_,
                         Vector2(static_cast<float>(screenBounds.getCenterProperty().X -
                                                    line_->getWidthProperty() / 2 - 10),
                                 100.0f + scrollYPos),
                         Color::White);
    }
}

inline void SelectOnlineGameScreen::EnterGameName(const std::string& description,
                                                  const std::string& defaultName)
{
    (void)Guide::BeginShowKeyboardInput(
        PlayerIndex::One, "Enter game name", description, defaultName,
        [this](System::IAsyncResult& result) { EnterGameNameDialogEnded(result); }, {});
}

inline void SelectOnlineGameScreen::EnterGameNameDialogEnded(System::IAsyncResult& result)
{
    const std::optional<std::string> res = Guide::EndShowKeyboardInput(&result);
    delete &result;

    if (!res.has_value()) {
        return;
    }

    if (StringUtility::IsNameValid(*res)) {
        if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
            network->NewGame(*res);
        }
    } else {
        EnterGameName("The name is not valid", *res);
    }
}

inline void SelectOnlineGameScreen::InstanceError()
{
    isConnecting_ = false;
    error_ = "Server unavailable";
    hasError_ = true;
}

inline void SelectOnlineGameScreen::InstanceRegistered(const BooleanEventArgs& e)
{
    isConnecting_ = false;

    if (e.Answer) {
        isConnected_ = true;
        if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
            network->GetAvailableGames();
        }
    } else {
        error_ = "Register failed";
        hasError_ = true;
    }
}

inline void SelectOnlineGameScreen::InstanceAvailableGamesArrived(
    const YachtAvailableGamesEventArgs& e)
{
    isSearching_ = false;
    searchAgain_->Enabled = !isSearching_;
    availableGames_ = e.AvailableGames;
}

inline void SelectOnlineGameScreen::ConnectClick()
{
    if (!isConnecting_ && selectedLine_.has_value() && availableGames_ != nullptr) {
        if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
            network->JoinGame(
                availableGames_->Games[static_cast<std::size_t>(*selectedLine_)].GameID);
        }
    }
}

inline void SelectOnlineGameScreen::InstanceJoinedGame(const BooleanEventArgs& e)
{
    if (!e.Answer) {
        (void)Guide::BeginShowMessageBox("Cannot join this game", " ",
                                         std::vector<std::string>{"OK"}, 0, MessageBoxIcon::Alert,
                                         nullptr, {});
        return;
    }

    auto& state =
        Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty().getStateProperty();
    state.Remove(Constants::YachtStateKey);
    state.Add(Constants::YachtStateKey, std::make_shared<YachtState>());

    ExitScreen();
    Dispose();
    getScreenManagerProperty()->AddScreen(
        std::make_shared<GameplayScreen>(YachtServices::GameTypes::Online), std::nullopt);
}

inline void SelectOnlineGameScreen::NewGameClick()
{
    EnterGameName("", "Game1");
}

inline void SelectOnlineGameScreen::InstanceNewGameCreated(const BooleanEventArgs& e)
{
    auto* network = NetworkManager::getInstanceProperty();

    if (!e.Answer) {
        EnterGameName("The name is in use", network != nullptr ? network->gameName : "");
        return;
    }

    auto& state =
        Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty().getStateProperty();
    state.Remove(Constants::YachtStateKey);
    state.Add(Constants::YachtStateKey, std::make_shared<YachtState>());

    ExitScreen();
    Dispose();
    getScreenManagerProperty()->AddScreen(
        std::make_shared<GameplayScreen>(YachtServices::GameTypes::Online), std::nullopt);
}

inline void SelectOnlineGameScreen::SearchAgainClick()
{
    auto* network = NetworkManager::getInstanceProperty();
    if (network == nullptr) {
        return;
    }

    if (!isConnected_) {
        network->Connect(name_);
        isConnecting_ = true;
        error_.clear();
        hasError_ = false;
    } else {
        isSearching_ = true;
        network->GetAvailableGames();
    }
}

} // namespace Yacht
