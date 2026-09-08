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

#include "ScreenManager/ScreenBodies.hpp"
#include "Screens.hpp"
#include "Misc/AudioManager.hpp"
#include "Accelerometer.hpp"
#include "Objects/DiceHandler.hpp"
#include "Microsoft/Phone/Shell/PhoneApplicationService.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFile.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFileStream.hpp"
#include "System/Xml/XmlReader.hpp"
#include "System/Xml/XmlWriter.hpp"

#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/MessageBoxIcon.hpp"

#include "Constants.hpp"
#include "Misc/NetworkManager.hpp"
#include "YachtState.hpp"
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

        AudioManager::Initialize(*this);

        // Subscribe to the application's lifecycle events. The extra line is the attach: on the
        // phone the operating system owned this service, and here the game is the only thing
        // that receives the platform's lifecycle transitions.
        auto& phone = Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty();
        phone.Activated += [this](System::Object* sender,
                                  const Microsoft::Phone::Shell::ActivatedEventArgs& e) {
            GameActivated(sender, e);
        };
        phone.Deactivated += [this](System::Object* sender,
                                    const Microsoft::Phone::Shell::DeactivatedEventArgs& e) {
            GameDeactivated(sender, e);
        };
        phone.Closing += [this](System::Object* sender,
                                const Microsoft::Phone::Shell::ClosingEventArgs& e) {
            GameClosed(sender, e);
        };
        phone.Launching += [this](System::Object* sender,
                                  const Microsoft::Phone::Shell::LaunchingEventArgs& e) {
            GameLaunched(sender, e);
        };

        // Attached last: attaching reports the fresh start, and a handler added afterwards
        // would miss it.
        phone.AttachEXT(*this);
    }

    const std::string& GetTypeName() const override {
        static const std::string name = "YachtGame";
        return name;
    }

private:
    // -- Tombstoning ------------------------------------------------------------------------

    /** Saves necessary data to isolated storage before the game is deactivated. */
    void GameDeactivated(System::Object*, const Microsoft::Phone::Shell::DeactivatedEventArgs&)
    {
        if (Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty()
                .getStateProperty()
                .ContainsKey(Constants::YachtStateKey)) {
            SaveGameState();
        }
    }

    /**
     * Loads game state data from isolated storage once the game is activated. If an online game
     * was in progress, reconnects to the server. If no stored data is available, starts the
     * game normally.
     */
    void GameActivated(System::Object*, const Microsoft::Phone::Shell::ActivatedEventArgs&)
    {
        // Check if we were in the middle of an online game
        if (LoadGameState(YachtServices::GameTypes::Online)) {
            // Remove stored online data
            DeleteIsolatedStorageFile(Constants::YachtStateFileNameOnline);

            // The network manager is updated according to the game state loaded so try to connect
            ReconnectToServer();
            return;
        }

        // Check if we were in the middle of an offline game
        if (LoadGameState(YachtServices::GameTypes::Offline)) {
            // Remove stored offline data
            DeleteIsolatedStorageFile(Constants::YachtStateFileNameOffline);

            screenManager_->AddScreen(
                std::make_shared<GameplayScreen>(YachtServices::GameTypes::Offline), std::nullopt);
            return;
        }

        // There is no game state data, so display the main menu
        screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    }

    /** Save the game state to isolated storage when closing the game. */
    void GameClosed(System::Object*, const Microsoft::Phone::Shell::ClosingEventArgs&)
    {
        if (Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty()
                .getStateProperty()
                .ContainsKey(Constants::YachtStateKey)) {
            SaveGameState();
        }
    }

    /** Moves to the main menu screen. */
    void GameLaunched(System::Object*, const Microsoft::Phone::Shell::LaunchingEventArgs&)
    {
        // Check if we were in the middle of an online game
        if (LoadGameState(YachtServices::GameTypes::Online)) {
            // Remove stored online data
            DeleteIsolatedStorageFile(Constants::YachtStateFileNameOnline);

            ReconnectToServer();
            return;
        }

        // Start the game normally, at the main menu
        screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    }

    // -- Server communication handlers ------------------------------------------------------

    void ReconnectToServer()
    {
        auto* network = NetworkManager::getInstanceProperty();
        if (network == nullptr) {
            screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
            return;
        }

        registeredToken_ = network->Registered.Add(
            [this](System::Object* sender, const BooleanEventArgs& e) {
                RegisteredWithServer(sender, e);
            });
        serviceErrorToken_ = network->ServiceError.Add(
            [this](System::Object* sender, const ExceptionEventArgs& e) {
                ServerErrorOccurred(sender, e);
            });
        network->Connect(network->name);
    }

    void UnsubscribeFromServerEvents()
    {
        if (auto* network = NetworkManager::getInstanceProperty(); network != nullptr) {
            network->Registered.Remove(registeredToken_);
            network->ServiceError.Remove(serviceErrorToken_);
        }
    }

    /** Called when there is an error contacting the game server. */
    void ServerErrorOccurred(System::Object*, const ExceptionEventArgs&)
    {
        // We no longer need to be notified of server events (the main menu screen will handle that)
        UnsubscribeFromServerEvents();

        screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);

        Microsoft::Xna::Framework::GamerServices::Guide::BeginShowMessageBox("The server is unavailable", "  ",
                                   std::vector<std::string>{"OK"}, 0, Microsoft::Xna::Framework::GamerServices::MessageBoxIcon::Alert,
                                   nullptr, nullptr);
    }

    /** Called once registration with the game server is successful. */
    void RegisteredWithServer(System::Object*, const BooleanEventArgs&)
    {
        // We no longer need to be notified of server events (the gameplay screen will handle that)
        UnsubscribeFromServerEvents();
        screenManager_->AddScreen(
            std::make_shared<GameplayScreen>(YachtServices::GameTypes::Online), std::nullopt);
    }

public:
    // -- Tombstoning ------------------------------------------------------------------------

    /**
     * @brief Saves the game-state data from the game's state object in isolated storage.
     *
     * Assumes the game state object contains game-state data.
     */
    static void SaveGameState()
    {
        auto& state = Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty()
                          .getStateProperty();
        if (!state.ContainsKey(Constants::YachtStateKey)) {
            return;
        }

        try {
            std::shared_ptr<System::Object> stored;
            if (!state.TryGetValue(Constants::YachtStateKey, stored)) {
                return;
            }
            const auto yachtState = std::dynamic_pointer_cast<YachtState>(stored);
            if (yachtState == nullptr) {
                return;
            }

            const std::string fileName =
                (yachtState->YachGameState != nullptr &&
                 yachtState->YachGameState->GameType == YachtServices::GameTypes::Offline)
                    ? Constants::YachtStateFileNameOffline
                    : Constants::YachtStateFileNameOnline;

            // The original writes through XmlWriter.Create(fileStream). This runtime's writer
            // builds its document in memory and hands back the text, so the text is written to
            // the stream instead; the file is the same either way.
            std::unique_ptr<System::Xml::XmlWriter> writer(System::Xml::XmlWriter::CreateToString());
            yachtState->WriteXml(*writer);
            const std::string document = writer->ToString();

            auto isolatedStorageFile =
                System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();
            auto fileStream = isolatedStorageFile.CreateFile(fileName);
            std::vector<SharpRuntime::bytecs> bytes(document.begin(), document.end());
            if (!bytes.empty()) {
                fileStream.Write(bytes.data(), 0, static_cast<SharpRuntime::intcs>(bytes.size()));
            }
        } catch (...) {
            // There was an error saving data to isolated storage. Not much that we can do about it.
        }
    }

    /**
     * @brief Loads a game-state object from isolated storage into the game's state object.
     *
     * @param gameType The type of game for which to load the data.
     * @return True if game data was successfully loaded and false otherwise.
     */
    static bool LoadGameState(YachtServices::GameTypes gameType)
    {
        const std::string fileName = gameType == YachtServices::GameTypes::Offline
                                         ? Constants::YachtStateFileNameOffline
                                         : Constants::YachtStateFileNameOnline;

        try {
            auto isolatedStorageFile =
                System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();

            // Check whether or not the data file exists
            if (!isolatedStorageFile.FileExists(fileName)) {
                return false;
            }

            // If the file exits, open it and read its contents
            auto fileStream =
                isolatedStorageFile.OpenFile(fileName, System::IO::FileMode::Open);
            const auto length = fileStream.getLengthProperty();
            std::vector<SharpRuntime::bytecs> bytes(static_cast<std::size_t>(length));
            if (length > 0) {
                (void)fileStream.Read(bytes.data(), 0, length);
            }

            std::unique_ptr<System::Xml::XmlReader> reader(
                System::Xml::XmlReader::CreateFromString(std::string(bytes.begin(), bytes.end())));

            auto yachtState = std::make_shared<YachtState>();

            // Read the xml declaration to get it out of the way
            reader->Read();

            yachtState->ReadXml(*reader);

            Microsoft::Phone::Shell::PhoneApplicationService::getCurrentProperty()
                .getStateProperty()
                .Add(Constants::YachtStateKey, yachtState);

            return true;
        } catch (...) {
            return false;
        }
    }

    /**
     * @brief Cleans a specific file from isolated storage.
     *
     * @param fileName The name of the file to clean from isolated storage.
     */
    static void DeleteIsolatedStorageFile(const std::string& fileName)
    {
        auto isolatedStorageFile =
            System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();
        if (isolatedStorageFile.FileExists(fileName)) {
            isolatedStorageFile.DeleteFile(fileName);
        }
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

    System::EventHandler<BooleanEventArgs>::Token registeredToken_ = 0;
    System::EventHandler<ExceptionEventArgs>::Token serviceErrorToken_ = 0;
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
