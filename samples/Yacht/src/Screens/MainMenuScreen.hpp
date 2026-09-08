#pragma once

// MainMenuScreen.hpp -- C++ port of Yacht/Screens/MainMenuScreen.cs.

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

#include "../Misc/StringUtility.hpp"
#include "../ScreenManager/MenuBodies.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::GamerServices::Guide;

/**
 * @brief The title screen: offline game, online game, exit.
 */
class MainMenuScreen : public MenuScreen {
public:
    /** @brief The type's name. @return "MainMenuScreen". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "MainMenuScreen";
        return name;
    }

    /** @brief Creates the screen. */
    MainMenuScreen() : MenuScreen("") {}

    /** @brief Load graphics content for the screen. */
    void LoadContent() override;

    /**
     * @brief Asks the player's name again when the last one was rejected.
     *
     * @param gameTime             The elapsed time.
     * @param otherScreenHasFocus  Whether another screen has focus.
     * @param coveredByOtherScreen Whether another screen covers this one.
     */
    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override;

    /**
     * @brief Draws the title art under the menu.
     *
     * @param gameTime The elapsed time.
     */
    void Draw(const GameTime& gameTime) override;

protected:
    /**
     * @brief Backing out of the main menu leaves the game.
     *
     * @param playerIndex Who cancelled.
     */
    void OnCancel(PlayerIndex playerIndex) override;

private:
    void OfflineGameMenuEntrySelected();
    void OnlineGameMenuEntrySelected();
    void EnterNameDialogEnded(System::IAsyncResult& result);

    std::optional<Texture2D> background_;
    std::optional<Texture2D> titleTexture_;
    Vector2 titlePosition_;
    bool isInvalidName_ = false;
    std::string invalidName_;
};

} // namespace Yacht
