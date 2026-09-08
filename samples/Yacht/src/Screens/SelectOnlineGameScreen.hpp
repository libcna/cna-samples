#pragma once

// SelectOnlineGameScreen.hpp -- C++ port of Yacht/Screens/SelectOnlineGameScreen.cs.

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/MessageBoxIcon.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../Misc/Button.hpp"
#include "../Misc/NetworkManager.hpp"
#include "../Misc/StringUtility.hpp"
#include "../ScreenManager/ScreenBodies.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::GamerServices::Guide;
using Microsoft::Xna::Framework::GamerServices::MessageBoxIcon;

/**
 * @brief The lobby: connect to the server, look at the games waiting for players, join one or
 *        start another.
 *
 * The screen is a state machine over the network manager's events rather than a request/reply
 * dialogue, because every call to the server answers later: the screen shows "Connecting...",
 * then whatever arrived.
 */
class SelectOnlineGameScreen : public GameScreen {
public:
    /** @brief The type's name. @return "SelectOnlineGameScreen". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "SelectOnlineGameScreen";
        return name;
    }

    /**
     * @brief Creates the screen and starts connecting.
     *
     * @param name The player's name, which is how the server knows them.
     */
    explicit SelectOnlineGameScreen(std::string name);

    /** @brief Unsubscribes from the server's events. */
    void Dispose();

    /** @brief Load graphics content for the screen. */
    void LoadContent() override;

    /**
     * @brief Handles the player's input.
     *
     * @param input This frame's input.
     */
    void HandleInput(InputState& input) override;

    /**
     * @brief Enables the buttons the current state allows.
     *
     * @param gameTime             The elapsed time.
     * @param otherScreenHasFocus  Whether another screen has focus.
     * @param coveredByOtherScreen Whether another screen covers this one.
     */
    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override;

    /**
     * @brief Draws the lobby.
     *
     * @param gameTime The elapsed time.
     */
    void Draw(const GameTime& gameTime) override;

private:
    void HandleDragList(const GestureSample& sample);
    void HandleSelectedGameInput(const GestureSample& sample);
    void DrawAvailableGames(SpriteBatch& spriteBatch);
    void EnterGameName(const std::string& description, const std::string& defaultName);
    void EnterGameNameDialogEnded(System::IAsyncResult& result);
    void InstanceError();
    void InstanceRegistered(const BooleanEventArgs& e);
    void InstanceAvailableGamesArrived(const YachtAvailableGamesEventArgs& e);
    void ConnectClick();
    void InstanceJoinedGame(const BooleanEventArgs& e);
    void NewGameClick();
    void InstanceNewGameCreated(const BooleanEventArgs& e);
    void SearchAgainClick();

    static constexpr int maxShowGames_ = 10;

    std::optional<Texture2D> background_;
    std::optional<Texture2D> line_;
    std::optional<Texture2D> scrollThumb_;
    std::optional<Texture2D> buttonTexture_;
    std::shared_ptr<YachtServices::AvailableGames> availableGames_;
    std::optional<Button> searchAgain_;
    std::optional<Button> newGame_;
    std::optional<Button> connect_;
    bool isConnecting_ = true;
    bool isConnected_ = false;
    bool isSearching_ = false;
    std::string error_;
    bool hasError_ = false;
    std::string name_;
    int frame_ = 300;
    std::string text_ = "Connecting...";
    Vector2 offset_ = Vector2::Zero;
    std::optional<int> selectedLine_;

    System::EventHandler<BooleanEventArgs>::Token registeredToken_ = 0;
    System::EventHandler<ExceptionEventArgs>::Token serviceErrorToken_ = 0;
    System::EventHandler<YachtAvailableGamesEventArgs>::Token availableGamesToken_ = 0;
    System::EventHandler<BooleanEventArgs>::Token newGameCreatedToken_ = 0;
    System::EventHandler<BooleanEventArgs>::Token joinedGameToken_ = 0;
};

} // namespace Yacht
