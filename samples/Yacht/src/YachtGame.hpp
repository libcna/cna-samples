#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "System/TimeSpan.hpp"

#include "ScreenManager.hpp"
#include "Screens.hpp"
#include "Misc/AudioManager.hpp"
#include "Accelerometer.hpp"
#include "Objects/DiceHandler.hpp"
#include "Misc/NetworkManager.hpp"
#include "Objects/NetworkPlayer.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::GraphicsDeviceManager;
using Microsoft::Xna::Framework::DisplayOrientation;
using Microsoft::Xna::Framework::Input::Keyboard;
using Microsoft::Xna::Framework::Input::Keys;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

// The Yacht dice game: a human player takes turns against three AI opponents, or against other
// people through the game server.
//
// The five fonts are static properties of the game because every screen draws with them and the
// original made them reachable without a content manager. They are loaded here, in LoadContent,
// exactly as the original does.
class YachtGame : public Game {
public:
    /** @brief The font used for ordinary text. */
    static SpriteFont* RegularFont;

    /** @brief The font used for score-card entries. */
    static SpriteFont* ScoreFont;

    /** @brief The bold font used to pick out a score-card entry. */
    static SpriteFont* ScoreFontBold;

    /** @brief The font used on the leaderboard. */
    static SpriteFont* LeaderScoreFont;

    /** @brief The font used for menu entries. */
    static SpriteFont* Font;

    YachtGame() {
        getContentProperty().setRootDirectoryProperty("Content");

        // The Windows Phone original ran at 30 fps.
        setTargetElapsedTimeProperty(System::TimeSpan::FromSeconds(1.0 / 30.0));

        graphics_ = std::make_unique<GraphicsDeviceManager>(this);
        graphics_->setIsFullScreenProperty(true);
        graphics_->setSupportedOrientationsProperty(DisplayOrientation::Portrait);
        graphics_->setPreferredBackBufferWidthProperty(480);
        graphics_->setPreferredBackBufferHeightProperty(800);

        screenManager_ = std::make_unique<ScreenManager>(*this);
        getComponentsProperty().Add(&*screenManager_);

        // The original opens the run from PhoneApplicationService's Launching or Activated
        // handler, which is where it decides whether to resume a stored game. Until YachtState
        // lands -- it serializes the network manager, so it arrives with the online half -- the
        // main menu is opened here, which is what Launching does when there is nothing stored.
        screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);

        AudioManager::Initialize(*this);
    }

    const std::string& GetTypeName() const override {
        static const std::string name = "YachtGame";
        return name;
    }

protected:
    void Initialize() override {
        // Confirmed gotcha: CNA does not auto-wire the touch panel's display
        // metrics from the back buffer size, so this must be done explicitly.
        TouchPanel::setDisplayWidthProperty(480);
        TouchPanel::setDisplayHeightProperty(800);
        TouchPanel::setDisplayOrientationProperty(DisplayOrientation::Portrait);

        // CNAEXT. The game is driven entirely by touch gestures, as a phone game is, and a
        // desktop has no touch screen -- so the platform turns the mouse into one rather than
        // the game growing a second, invented input path beside the original's.
        TouchPanel::setMouseTouchEmulationEnabledEXT(true);

        Accelerometer::Initialize();

        Game::Initialize();
    }

    void LoadContent() override {
        AudioManager::LoadSounds();

        regularFont_.emplace(getContentProperty().Load<SpriteFont>("Fonts/Regular"));
        scoreFont_.emplace(getContentProperty().Load<SpriteFont>("Fonts/ScoreFont"));
        scoreFontBold_.emplace(getContentProperty().Load<SpriteFont>("Fonts/ScoreFontBold"));
        leaderScoreFont_.emplace(getContentProperty().Load<SpriteFont>("Fonts/LeaderScoreFont"));
        font_.emplace(getContentProperty().Load<SpriteFont>("Fonts/MenuFont"));

        RegularFont = &*regularFont_;
        ScoreFont = &*scoreFont_;
        ScoreFontBold = &*scoreFontBold_;
        LeaderScoreFont = &*leaderScoreFont_;
        Font = &*font_;

        Game::LoadContent();
    }

    void Update(GameTime& gameTime) override {
        // The proxy answers on a worker thread and the push channel listens on another, and
        // neither raises anything where it happens: both queue, and this is where the queues
        // are drained. Windows Phone's dispatcher did the same job for the original, which is
        // why its handlers could touch game state without a thought.
        if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
            network->DispatchPendingCompletions();
        }

        Game::Update(gameTime);
    }

    void Draw(const GameTime& gameTime) override {
        getGraphicsDeviceProperty().Clear(Color::Black);

        // The real drawing happens inside the screen manager component.
        Game::Draw(gameTime);
    }

private:
    std::unique_ptr<GraphicsDeviceManager> graphics_;
    std::unique_ptr<ScreenManager> screenManager_;

    // The fonts themselves; the static properties above point into these, so the game owns them
    // and every screen borrows them, which is the lifetime the original's static properties have.
    std::optional<SpriteFont> regularFont_;
    std::optional<SpriteFont> scoreFont_;
    std::optional<SpriteFont> scoreFontBold_;
    std::optional<SpriteFont> leaderScoreFont_;
    std::optional<SpriteFont> font_;
};

// The network player's waiting message is measured and drawn with the game's font, so both of
// its bodies close here for the same reason DiceHandler::Draw does.
inline NetworkPlayer::NetworkPlayer(std::string name, const Rectangle& screenBounds)
    : YachtPlayer(std::move(name), nullptr)
{
    const Vector2 measure = YachtGame::Font->MeasureString(text_);
    position_ = Vector2(static_cast<float>(screenBounds.getCenterProperty().X) - measure.X / 2,
                        static_cast<float>(screenBounds.getBottomProperty()) - 70);
}

inline void NetworkPlayer::Draw(SpriteBatch& spriteBatch)
{
    spriteBatch.DrawString(*YachtGame::Font, text_, position_, Color::White);
    YachtPlayer::Draw(spriteBatch);
}

// Out-of-line because DiceHandler cannot include this header: the game reaches the dice
// through the screens, so the reference only closes here, once both classes are complete.
inline void DiceHandler::Draw(SpriteBatch& spriteBatch) const
{
    spriteBatch.Draw(*holdingTray_, holdingTrayPosition_, Color::White);
    spriteBatch.DrawString(*YachtGame::Font, "HOLD", holdTextPosition_, Color::White);
    spriteBatch.Draw(*diceRollBorder_, rollBorderPosition_, Color::White);

    DrawDice(spriteBatch);
}

inline SpriteFont* YachtGame::RegularFont = nullptr;
inline SpriteFont* YachtGame::ScoreFont = nullptr;
inline SpriteFont* YachtGame::ScoreFontBold = nullptr;
inline SpriteFont* YachtGame::LeaderScoreFont = nullptr;
inline SpriteFont* YachtGame::Font = nullptr;

} // namespace Yacht
