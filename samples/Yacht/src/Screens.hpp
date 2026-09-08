#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Random.hpp"

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "ScreenManager/MenuBodies.hpp"
#include "GameStateHandler.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;

// ===================== MessageBoxScreen =====================
// A popup message box used for "are you sure?" confirmation prompts, ported
// near-verbatim from the GameStateManagement/CatapultWars precedent (the
// original Yacht used Guide.BeginShowMessageBox, which has no CNA
// equivalent). Used here to replace the original's "Are you sure you want
// to leave the game?" prompt in GameplayScreen; the original's other
// message-box usages (save-game prompt, online server-error dialogs) are
// dropped along with tombstoning/online play.
class MessageBoxScreen : public GameScreen {
public:
    /** @brief The type's name. @return "MessageBoxScreen". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "MessageBoxScreen";
        return name;
    }

    std::function<void(PlayerIndex)> Accepted;
    std::function<void(PlayerIndex)> Cancelled;

    explicit MessageBoxScreen(const std::string& message) : message_(message) {
        setIsPopupProperty(true);
        setTransitionOnTimeProperty(TimeSpan::FromSeconds(0.2));
        setTransitionOffTimeProperty(TimeSpan::FromSeconds(0.2));
    }

    void LoadContent() override {
        backgroundTexture_.emplace(Load<Texture2D>("Images/button"));
    }

    void HandleInput(InputState& input) override {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(getControllingPlayerProperty(), playerIndex)) {
            if (Accepted) Accepted(playerIndex);
            ExitScreen();
        } else if (input.IsMenuCancel(getControllingPlayerProperty(), playerIndex)) {
            if (Cancelled) Cancelled(playerIndex);
            ExitScreen();
        }
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = getScreenManagerProperty()->getSpriteBatchProperty();
        SpriteFont& font = getScreenManagerProperty()->getFontProperty();

        // Darken down any other screens that were drawn beneath the popup.
        getScreenManagerProperty()->FadeBackBufferToBlack(getTransitionAlphaProperty() * 2 / 3);

        auto& viewport = getScreenManagerProperty()->getGraphicsDeviceProperty().getViewportProperty();
        Vector2 textSize = font.MeasureString(message_);
        Vector2 textPosition(((float)viewport.getWidthProperty() - textSize.X) / 2,
                             ((float)viewport.getHeightProperty() - textSize.Y) / 2);

        const int hPad = 32;
        const int vPad = 16;

        Rectangle backgroundRectangle((int)textPosition.X - hPad, (int)textPosition.Y - vPad,
                                      (int)textSize.X + hPad * 2, (int)textSize.Y + vPad * 2);

        Color color = Color::White * getTransitionAlphaProperty();

        spriteBatch.Begin();
        spriteBatch.Draw(*backgroundTexture_, backgroundRectangle, color);
        spriteBatch.DrawString(font, message_, textPosition, color);
        spriteBatch.End();
    }

private:
    std::string message_;
    std::optional<Texture2D> backgroundTexture_;
};

// ===================== GameplayScreen =====================
// The main gameplay screen. Ported from Screens/GameplayScreen.cs; the
// server-communication handlers, Guide.IsVisible gating (the ScreenManager
// framework already only calls HandleInput on the topmost active screen, so
// a popup naturally takes over input for free) and tombstoning are all
// dropped per the approved plan.
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
    explicit GameplayScreen(YachtServices::GameTypes gameType) : gameType_(gameType) {
        setEnabledGesturesProperty(GestureType::Tap | GestureType::VerticalDrag | GestureType::DragComplete);
    }

    /**
     * @brief Initialize a new game screen.
     *
     * @param name     The name of the human player participating in the game.
     * @param gameType The type of game for which this screen is created.
     */
    GameplayScreen(const std::string& name, YachtServices::GameTypes gameType)
        : GameplayScreen(gameType) {
        name_ = name;
    }

    void LoadContent() override {
        background_.emplace(Load<Texture2D>("Images/bg"));
        Dice::LoadAssets(getScreenManagerProperty()->getGameProperty().getContentProperty());

        regularFont_.emplace(Load<SpriteFont>("Fonts/Regular"));
        scoreFont_.emplace(Load<SpriteFont>("Fonts/ScoreFont"));
        scoreFontBold_.emplace(Load<SpriteFont>("Fonts/ScoreFontBold"));
        leaderScoreFont_.emplace(Load<SpriteFont>("Fonts/LeaderScoreFont"));
        font_.emplace(Load<SpriteFont>("Fonts/MenuFont"));

        auto& graphicsDevice = getScreenManagerProperty()->getGraphicsDeviceProperty();
        Rectangle screenBounds = graphicsDevice.getViewportProperty().getBoundsProperty();

        diceHandler_ = std::make_unique<DiceHandler>(graphicsDevice, nullptr);
        diceHandler_->LoadAssets(getScreenManagerProperty()->getGameProperty().getContentProperty());

        gameStateHandler_ = std::make_unique<GameStateHandler>(
            *diceHandler_, getScreenManagerProperty()->input, name_, screenBounds, getScreenManagerProperty()->getGameProperty().getContentProperty(), *font_,
            ScoreFonts{&*regularFont_, &*scoreFont_, &*scoreFontBold_, &*leaderScoreFont_});
    }

    // Defined at the bottom of this file (needs MainMenuScreen/MessageBoxScreen).
    void HandleInput(InputState& input) override;

    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override {
        if (gameStateHandler_ && !gameStateHandler_->IsGameOver()) {
            {
                auto* held = diceHandler_->GetHoldingDice();
                gameStateHandler_->setScoreDice(
                    held == nullptr ? std::nullopt
                                    : std::optional(GameStateHandler::ToRawDice(*held)));
            }
            diceHandler_->Update();

            if (gameStateHandler_->IsInitialized()) {
                YachtPlayer* current = gameStateHandler_->CurrentPlayer();

                if (dynamic_cast<AIPlayer*>(current) == nullptr) {
                    current->PerformPlayerLogic();
                    aiTimerArmed_ = false;
                } else {
                    float elapsed = (float)gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty();
                    if (!aiTimerArmed_) {
                        aiThinkTimer_ = random_.Next(300, 600) / 1000.0f;
                        aiTimerArmed_ = true;
                    } else {
                        aiThinkTimer_ -= elapsed;
                        if (aiThinkTimer_ <= 0.0f) {
                            aiTimerArmed_ = false;
                            current->PerformPlayerLogic();
                        }
                    }
                }
            }
        }

        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        getScreenManagerProperty()->getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        SpriteBatch& spriteBatch = getScreenManagerProperty()->getSpriteBatchProperty();
        spriteBatch.Begin();

        spriteBatch.Draw(*background_, Vector2::Zero, Color::White);

        if (gameStateHandler_ && gameStateHandler_->IsInitialized()) {
            if (!gameStateHandler_->IsGameOver()) {
                diceHandler_->Draw(spriteBatch);
                gameStateHandler_->CurrentPlayer()->Draw(spriteBatch);
            }
            gameStateHandler_->Draw(spriteBatch);
        }

        DrawGameOver(spriteBatch);

        spriteBatch.End();
    }

private:
    void DrawGameOver(SpriteBatch& spriteBatch) {
        if (gameStateHandler_ && gameStateHandler_->IsGameOver()) {
            auto& viewport = getScreenManagerProperty()->getGraphicsDeviceProperty().getViewportProperty();
            std::string winnerText = gameStateHandler_->WinnerPlayer()->getNameProperty() + " is the winner!";
            Vector2 measure = font_->MeasureString(winnerText);
            Vector2 position((float)(viewport.getWidthProperty() / 2) - measure.X / 2.0f,
                             (float)viewport.getHeightProperty() - 100.0f);
            spriteBatch.DrawString(*font_, winnerText, position, Color::White);
        }
    }

    // Defined at the bottom of this file (needs MessageBoxScreen/MainMenuScreen).
    void QuitGame();

    std::string name_;
    YachtServices::GameTypes gameType_ = YachtServices::GameTypes::Offline;
    std::optional<Texture2D> background_;
    std::optional<SpriteFont> regularFont_, scoreFont_, scoreFontBold_, leaderScoreFont_, font_;

    std::unique_ptr<DiceHandler> diceHandler_;
    std::unique_ptr<GameStateHandler> gameStateHandler_;

    System::Random random_;
    bool aiTimerArmed_ = false;
    float aiThinkTimer_ = 0.0f;
};

// ===================== InstructionScreen =====================
// Displays the game instructions; ported from Screens/InstructionScreen.cs.
// Guide.BeginShowKeyboardInput has no CNA equivalent, so per the approved
// plan the player name defaults to the fixed literal "Player1" (matching a
// placeholder the original itself already used on its online path) instead
// of prompting for a name.
class InstructionScreen : public GameScreen {
public:
    /** @brief The type's name. @return "InstructionScreen". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "InstructionScreen";
        return name;
    }

    InstructionScreen() {
        setTransitionOnTimeProperty(TimeSpan::FromSeconds(0.0));
        setTransitionOffTimeProperty(TimeSpan::FromSeconds(0.5));
        setEnabledGesturesProperty(GestureType::Tap);
    }

    void LoadContent() override {
        background_.emplace(Load<Texture2D>("Images/instruction"));
        font_.emplace(Load<SpriteFont>("Fonts/MenuFont"));
    }

    // Defined at the bottom of this file (needs MainMenuScreen).
    void HandleInput(InputState& input) override;

    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override {
        if (isExit_ && !screenExited_) {
            // Move on to the gameplay screen.
            for (auto& screen : getScreenManagerProperty()->GetScreens())
                screen->ExitScreen();

            getScreenManagerProperty()->AddScreen(std::make_shared<GameplayScreen>("Player1", YachtServices::GameTypes::Offline), std::nullopt);

            screenExited_ = true;
        }

        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    }

    void Draw(const GameTime& gameTime) override {
        (void)gameTime;
        SpriteBatch& spriteBatch = getScreenManagerProperty()->getSpriteBatchProperty();
        spriteBatch.Begin();

        auto bounds = getScreenManagerProperty()->getGraphicsDeviceProperty().getViewportProperty().getBoundsProperty();
        spriteBatch.Draw(*background_, bounds, Color::White * getTransitionAlphaProperty());

        if (isExit_) {
            Rectangle safeArea = getScreenManagerProperty()->getSafeAreaProperty();
            std::string text = "Loading...";
            Vector2 measure = font_->MeasureString(text);
            Vector2 textPosition((float)safeArea.getCenterProperty().X - measure.X / 2.0f,
                                 (float)safeArea.getCenterProperty().Y - measure.Y / 2.0f);
            spriteBatch.DrawString(*font_, text, textPosition, Color::Black);
        }

        spriteBatch.End();
    }

    bool IsExit() const { return isExit_; }
    void setExit() { isExit_ = true; }

private:
    std::optional<Texture2D> background_;
    std::optional<SpriteFont> font_;
    bool isExit_ = false;
    bool screenExited_ = false;
};

// ===================== NewGameSubMenuScreen =====================
// Ported from Screens/NewGameSubMenuScreen.cs. The original's "Load" entry
// depended on the tombstoning save file that this port drops entirely, so
// only "New Game" remains (per the approved plan's screen-flow
// simplification: Offline Game always leads here now).
class NewGameSubMenuScreen : public MenuScreen {
public:
    /** @brief The type's name. @return "NewGameSubMenuScreen". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "NewGameSubMenuScreen";
        return name;
    }

    NewGameSubMenuScreen() : MenuScreen("") {}

    void LoadContent() override {
        background_.emplace(Load<Texture2D>("Images/bg"));

        auto newGameMenuEntry = std::make_shared<MenuEntry>("New Game");

        float screenWidth = (float)getScreenManagerProperty()->getGraphicsDeviceProperty().getViewportProperty().getWidthProperty();
        float screenHeight = (float)getScreenManagerProperty()->getGraphicsDeviceProperty().getViewportProperty().getHeightProperty();

        // The original placed "New Game" above center (height/2 - 40) to
        // leave room for a "Load" entry below it at height/2 + 40; with
        // "Load" dropped (see missing.md), "New Game" is centered instead.
        newGameMenuEntry->setDestinationProperty(
            Rectangle((int)screenWidth / 2 - 75, (int)screenHeight / 2 - 20, 150, 40));

        newGameMenuEntry->Selected += [this](System::Object*, const PlayerIndexEventArgs&) {
            for (auto& screen : getScreenManagerProperty()->GetScreens())
                screen->ExitScreen();

            getScreenManagerProperty()->AddScreen(std::make_shared<InstructionScreen>(), std::nullopt);
        };

        MenuEntries().push_back(newGameMenuEntry);

        MenuScreen::LoadContent();
    }

    void Draw(const GameTime& gameTime) override {
        SpriteBatch& spriteBatch = getScreenManagerProperty()->getSpriteBatchProperty();
        spriteBatch.Begin();
        spriteBatch.Draw(*background_, Vector2::Zero, Color::White);
        spriteBatch.End();

        MenuScreen::Draw(gameTime);
    }

protected:
    // Defined at the bottom of this file (needs MainMenuScreen).
    void OnCancel(PlayerIndex playerIndex) override;

private:
    std::optional<Texture2D> background_;
};

// ===================== MainMenuScreen =====================
// Ported from Screens/MainMenuScreen.cs. Per the approved plan, the
// "Online Game" entry is dropped entirely (no CNA/desktop equivalent to the
// WCF game server), so "Offline Game" is the only way into a game, and
// always goes to NewGameSubMenuScreen (the original only did this when a
// saved game existed to offer a "Load" choice from).
class MainMenuScreen : public MenuScreen {
public:
    /** @brief The type's name. @return "MainMenuScreen". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "MainMenuScreen";
        return name;
    }

    MainMenuScreen() : MenuScreen("") {}

    void LoadContent() override {
        background_.emplace(Load<Texture2D>("Images/titlescreen"));
        titleTexture_.emplace(Load<Texture2D>("Images/yachtTitle"));

        titlePosition_ = Vector2(
            (float)(getScreenManagerProperty()->getGraphicsDeviceProperty().getViewportProperty().getWidthProperty() / 2 -
                   titleTexture_->getWidthProperty() / 2),
            20.0f);

        auto offlineGameMenuEntry = std::make_shared<MenuEntry>("Offline Game");
        auto exitMenuEntry = std::make_shared<MenuEntry>("Exit");

        // The original also had an "Online Game" entry between these two, at
        // +80; with it dropped (see missing.md), Exit moves up to +80 to
        // close the resulting gap instead of leaving dead space.
        offlineGameMenuEntry->setDestinationProperty(Rectangle(
            30, (int)titlePosition_.Y + titleTexture_->getHeightProperty() + 20, 165, 55));
        exitMenuEntry->setDestinationProperty(Rectangle(
            30, (int)titlePosition_.Y + titleTexture_->getHeightProperty() + 80, 165, 45));

        offlineGameMenuEntry->Selected += [this](System::Object*, const PlayerIndexEventArgs&) {
            for (auto& screen : getScreenManagerProperty()->GetScreens())
                screen->ExitScreen();

            getScreenManagerProperty()->AddScreen(std::make_shared<NewGameSubMenuScreen>(), std::nullopt);
        };
        exitMenuEntry->Selected += [this](System::Object*, const PlayerIndexEventArgs& e) { OnCancel(e.getPlayerIndexProperty()); };

        MenuEntries().push_back(offlineGameMenuEntry);
        MenuEntries().push_back(exitMenuEntry);

        MenuScreen::LoadContent();
    }

    void Draw(const GameTime& gameTime) override {
        SpriteBatch& spriteBatch = getScreenManagerProperty()->getSpriteBatchProperty();
        spriteBatch.Begin();
        spriteBatch.Draw(*background_, Vector2::Zero, Color::White);
        spriteBatch.Draw(*titleTexture_, titlePosition_, Color::White);
        spriteBatch.End();

        MenuScreen::Draw(gameTime);
    }

protected:
    void OnCancel(PlayerIndex /*playerIndex*/) override {
        getScreenManagerProperty()->getGameProperty().Exit();
    }

private:
    std::optional<Texture2D> background_;
    std::optional<Texture2D> titleTexture_;
    Vector2 titlePosition_;
};

// ---- cross-referencing method definitions (after all screens declared) ----

inline void GameplayScreen::HandleInput(InputState& input) {
    // Deliberately has no early returns anywhere in this method, matching
    // the original: it never returns after QuitGame() either, and the
    // game-over tap-to-exit check falls through to the general gesture/
    // mouse forwarding loop below (so e.g. the scorecard can still be
    // scrolled after the game ends). The ScreenManager framework already
    // routes HandleInput only to the topmost active screen, so once
    // QuitGame() pushes a MessageBoxScreen popup this screen simply stops
    // receiving HandleInput starting next frame.
    if (input.IsPauseGame(std::nullopt)) {
        QuitGame();
    }

    if (gameStateHandler_ && gameStateHandler_->IsGameOver()) {
        bool tappedToExit = (!input.Gestures.empty() &&
                             input.Gestures[0].getGestureTypeProperty() == GestureType::Tap);
        if (tappedToExit) {
            ExitScreen();
            getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
        }
    }

    if (gameStateHandler_ && gameStateHandler_->IsInitialized()) {
        for (const auto& gesture : input.Gestures)
            gameStateHandler_->HandleInput(gesture);
            }
}

inline void GameplayScreen::QuitGame() {
    auto confirmQuit = std::make_shared<MessageBoxScreen>("Are you sure you want to quit?");
    confirmQuit->Accepted = [this](PlayerIndex) {
        ExitScreen();
        getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    };
    getScreenManagerProperty()->AddScreen(confirmQuit, getControllingPlayerProperty());
}

inline void InstructionScreen::HandleInput(InputState& input) {
    // No early return here either, matching the original exactly.
    if (input.IsPauseGame(std::nullopt)) {
        ExitScreen();
        getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    }

    if (!isExit_) {
        bool tapped = (!input.Gestures.empty() && input.Gestures[0].getGestureTypeProperty() == GestureType::Tap);
        if (tapped) {
            setExit();
        }
    }
}

inline void NewGameSubMenuScreen::OnCancel(PlayerIndex /*playerIndex*/) {
    for (auto& screen : getScreenManagerProperty()->GetScreens())
        screen->ExitScreen();

    getScreenManagerProperty()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
}

} // namespace Yacht
