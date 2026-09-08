#pragma once

// ScreenBodies.hpp -- the screens' bodies.
//
// Every screen pushes another when it is done, so they all reference each other; and several
// draw with the game's fonts. The declarations live in their own headers and the bodies close
// here, once every screen and the game are complete. Included from the end of YachtGame.hpp.

#include "../YachtGame.hpp"
#include "GameplayScreen.hpp"
#include "InstructionScreen.hpp"
#include "MainMenuScreen.hpp"
#include "NewGameSubMenuScreen.hpp"
#include "SelectOnlineGameScreen.hpp"

namespace Yacht {

// ---- InstructionScreen --------------------------------------------------------------------

inline void InstructionScreen::HandleInput(InputState& input)
{
    if (input.IsPauseGame(std::nullopt)) {
        ExitScreen();
        getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    }

    if (isExit_) {
        return;
    }

    if (!input.Gestures.empty() &&
        input.Gestures[0].getGestureTypeProperty() == GestureType::Tap) {
        if (askName_) {
            (void)Guide::BeginShowKeyboardInput(
                PlayerIndex::One, "Enter your name", "", "Player1",
                [this](System::IAsyncResult& result) { EnterNameDialogEnded(result); }, {});
        } else {
            isExit_ = true;
        }
    }
}

inline void InstructionScreen::EnterNameDialogEnded(System::IAsyncResult& result)
{
    const std::optional<std::string> entered = Guide::EndShowKeyboardInput(&result);
    delete &result;

    if (!entered.has_value()) {
        ExitScreen();
        getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
        return;
    }

    name_ = *entered;

    if (StringUtility::IsNameValid(name_)) {
        isExit_ = true;
    } else {
        isInvalidName_ = true;
        invalidName_ = name_;
    }
}

inline void InstructionScreen::Update(GameTime& gameTime, bool otherScreenHasFocus,
                                      bool coveredByOtherScreen)
{
    if (isInvalidName_) {
        isInvalidName_ = false;
        (void)Guide::BeginShowKeyboardInput(
            PlayerIndex::One, "Enter your name", "The name is not valid.", invalidName_,
            [this](System::IAsyncResult& result) { EnterNameDialogEnded(result); }, {});
        return;
    }

    if (isExit_ && !screenExited_) {
        for (const auto& screen : getScreenManagerProperty()->GetScreens()) {
            screen->ExitScreen();
        }
        getScreenManagerProperty()->AddScreen(
            std::make_shared<GameplayScreen>(name_, YachtServices::GameTypes::Offline),
            std::nullopt);
        screenExited_ = true;
    }

    GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
}

inline void InstructionScreen::Draw(const GameTime& gameTime)
{
    auto* screenManager = getScreenManagerProperty();
    SpriteBatch& spriteBatch = screenManager->getSpriteBatchProperty();

    spriteBatch.Begin();
    spriteBatch.Draw(*background_,
                     screenManager->getGraphicsDeviceProperty().getViewportProperty()
                         .getBoundsProperty(),
                     Color::White * getTransitionAlphaProperty());

    if (isExit_) {
        const Rectangle safeArea = screenManager->getSafeAreaProperty();
        const std::string text = "Loading...";
        const Vector2 measure = font_->MeasureString(text);
        const Vector2 textPosition(
            static_cast<float>(safeArea.getCenterProperty().X) - measure.X / 2,
            static_cast<float>(safeArea.getCenterProperty().Y) - measure.Y / 2);
        spriteBatch.DrawString(*font_, text, textPosition, Color::Black);
    }

    spriteBatch.End();

    GameScreen::Draw(gameTime);
}

// ---- NewGameSubMenuScreen -----------------------------------------------------------------

inline void NewGameSubMenuScreen::LoadContent()
{
    background_.emplace(Load<Texture2D>("Images/bg"));

    auto newGameMenuEntry = std::make_shared<MenuEntry>("New Game");
    auto loadGameMenuEntry = std::make_shared<MenuEntry>("Load");

    const auto screenWidth = static_cast<float>(getScreenManagerProperty()
                                                    ->getGraphicsDeviceProperty()
                                                    .getViewportProperty()
                                                    .getWidthProperty());
    const auto screenHeight = static_cast<float>(getScreenManagerProperty()
                                                     ->getGraphicsDeviceProperty()
                                                     .getViewportProperty()
                                                     .getHeightProperty());

    newGameMenuEntry->setDestinationProperty(Rectangle(static_cast<int>(screenWidth) / 2 - 75,
                                                       static_cast<int>(screenHeight) / 2 - 40,
                                                       150, 40));
    loadGameMenuEntry->setDestinationProperty(Rectangle(static_cast<int>(screenWidth) / 2 - 75,
                                                        static_cast<int>(screenHeight) / 2 + 40,
                                                        150, 40));

    newGameMenuEntry->Selected += [this](System::Object*, const PlayerIndexEventArgs&) {
        NewGameMenuEntrySelected();
    };
    loadGameMenuEntry->Selected += [this](System::Object*, const PlayerIndexEventArgs&) {
        LoadGameMenuEntrySelected();
    };

    MenuEntries().push_back(newGameMenuEntry);
    MenuEntries().push_back(loadGameMenuEntry);

    MenuScreen::LoadContent();
}

inline void NewGameSubMenuScreen::Draw(const GameTime& gameTime)
{
    SpriteBatch& spriteBatch = getScreenManagerProperty()->getSpriteBatchProperty();
    spriteBatch.Begin();
    spriteBatch.Draw(*background_, Vector2::Zero, Color::White);
    spriteBatch.End();

    MenuScreen::Draw(gameTime);
}

inline void NewGameSubMenuScreen::LoadGameMenuEntrySelected()
{
    for (const auto& screen : getScreenManagerProperty()->GetScreens()) {
        screen->ExitScreen();
    }
    getScreenManagerProperty()->AddScreen(std::make_shared<InstructionScreen>(false),
                                          std::nullopt);
}

inline void NewGameSubMenuScreen::NewGameMenuEntrySelected()
{
    auto& state =
        Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty().getStateProperty();

    if (state.ContainsKey("ScoreCard")) {
        state.Remove("ScoreCard");
        state.Remove("DiceHandler");
    } else if (state.ContainsKey("GameState")) {
        state.Remove("GameState");
    }

    for (const auto& screen : getScreenManagerProperty()->GetScreens()) {
        screen->ExitScreen();
    }
    getScreenManagerProperty()->AddScreen(std::make_shared<InstructionScreen>(true), std::nullopt);
}

inline void NewGameSubMenuScreen::OnCancel(PlayerIndex)
{
    for (const auto& screen : getScreenManagerProperty()->GetScreens()) {
        screen->ExitScreen();
    }
    getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
}

// ---- MainMenuScreen -----------------------------------------------------------------------

inline void MainMenuScreen::LoadContent()
{
    auto& content = getScreenManagerProperty()->getGameProperty().getContentProperty();
    background_.emplace(content.template Load<Texture2D>("Images/titlescreen"));
    titleTexture_.emplace(content.template Load<Texture2D>("Images/yachtTitle"));

    titlePosition_ = Vector2(
        static_cast<float>(getScreenManagerProperty()->getGraphicsDeviceProperty()
                               .getViewportProperty().getWidthProperty() / 2 -
                           titleTexture_->getWidthProperty() / 2),
        20.0f);

    auto offlineGameMenuEntry = std::make_shared<MenuEntry>("Offline Game");
    auto onlineGameMenuEntry = std::make_shared<MenuEntry>("Online Game");
    auto exitMenuEntry = std::make_shared<MenuEntry>("Exit");

    const int titleBottom =
        static_cast<int>(titlePosition_.Y) + titleTexture_->getHeightProperty();
    offlineGameMenuEntry->setDestinationProperty(Rectangle(30, titleBottom + 20, 165, 55));
    onlineGameMenuEntry->setDestinationProperty(Rectangle(30, titleBottom + 80, 165, 55));
    exitMenuEntry->setDestinationProperty(Rectangle(30, titleBottom + 140, 165, 45));

    offlineGameMenuEntry->Selected += [this](System::Object*, const PlayerIndexEventArgs&) {
        OfflineGameMenuEntrySelected();
    };
    onlineGameMenuEntry->Selected += [this](System::Object*, const PlayerIndexEventArgs&) {
        OnlineGameMenuEntrySelected();
    };
    exitMenuEntry->Selected += [this](System::Object*, const PlayerIndexEventArgs& e) {
        OnCancel(e.getPlayerIndexProperty());
    };

    MenuEntries().push_back(offlineGameMenuEntry);
    MenuEntries().push_back(onlineGameMenuEntry);
    MenuEntries().push_back(exitMenuEntry);

    MenuScreen::LoadContent();
}

inline void MainMenuScreen::Update(GameTime& gameTime, bool otherScreenHasFocus,
                                   bool coveredByOtherScreen)
{
    MenuScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

    if (isInvalidName_) {
        isInvalidName_ = false;
        (void)Guide::BeginShowKeyboardInput(
            PlayerIndex::One, "Enter your name", "The name is not valid.", invalidName_,
            [this](System::IAsyncResult& result) { EnterNameDialogEnded(result); }, {});
    }
}

inline void MainMenuScreen::OfflineGameMenuEntrySelected()
{
    for (const auto& screen : getScreenManagerProperty()->GetScreens()) {
        screen->ExitScreen();
    }

    if (YachtGame::LoadGameState(YachtServices::GameTypes::Offline)) {
        YachtGame::DeleteIsolatedStorageFile(Constants::YachtStateFileNameOffline);
        getScreenManagerProperty()->AddScreen(std::make_shared<NewGameSubMenuScreen>(),
                                              std::nullopt);
    } else {
        auto& state = Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty()
                          .getStateProperty();
        state.Remove(Constants::YachtStateKey);
        state.Add(Constants::YachtStateKey, std::make_shared<YachtState>());
        getScreenManagerProperty()->AddScreen(std::make_shared<InstructionScreen>(true),
                                              std::nullopt);
    }
}

inline void MainMenuScreen::OnlineGameMenuEntrySelected()
{
    auto* network = NetworkManager::getInstanceProperty();

    if (network == nullptr || network->name.empty()) {
        (void)Guide::BeginShowKeyboardInput(
            PlayerIndex::One, "Enter your name", "", "Player1",
            [this](System::IAsyncResult& result) { EnterNameDialogEnded(result); }, {});
        return;
    }

    ExitScreen();
    getScreenManagerProperty()->AddScreen(
        std::make_shared<SelectOnlineGameScreen>(network->name), std::nullopt);
}

inline void MainMenuScreen::EnterNameDialogEnded(System::IAsyncResult& result)
{
    const std::optional<std::string> entered = Guide::EndShowKeyboardInput(&result);
    delete &result;

    if (!entered.has_value()) {
        return;
    }

    if (StringUtility::IsNameValid(*entered)) {
        ExitScreen();
        getScreenManagerProperty()->AddScreen(std::make_shared<SelectOnlineGameScreen>(*entered),
                                              std::nullopt);
    } else {
        isInvalidName_ = true;
        invalidName_ = *entered;
    }
}

inline void MainMenuScreen::OnCancel(PlayerIndex)
{
    getScreenManagerProperty()->getGameProperty().Exit();
}

inline void MainMenuScreen::Draw(const GameTime& gameTime)
{
    SpriteBatch& spriteBatch = getScreenManagerProperty()->getSpriteBatchProperty();

    spriteBatch.Begin();
    spriteBatch.Draw(*background_, Vector2::Zero, Color::White);
    spriteBatch.Draw(*titleTexture_, titlePosition_, Color::White);
    spriteBatch.End();

    MenuScreen::Draw(gameTime);
}

} // namespace Yacht
