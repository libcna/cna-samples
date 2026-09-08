#pragma once

// NewGameSubMenuScreen.hpp -- C++ port of Yacht/Screens/NewGameSubMenuScreen.cs.

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Phone/Shell/PhoneApplicationService.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

#include "../ScreenManager/MenuBodies.hpp"

namespace Yacht {

/**
 * @brief The choice a returning player gets: start again, or pick up the stored game.
 *
 * Only shown when there is something stored -- a fresh player goes straight to the rules.
 */
class NewGameSubMenuScreen : public MenuScreen {
public:
    /** @brief The type's name. @return "NewGameSubMenuScreen". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "NewGameSubMenuScreen";
        return name;
    }

    /** @brief Creates the screen. */
    NewGameSubMenuScreen() : MenuScreen("") {}

    /** @brief Load graphics content for the screen. */
    void LoadContent() override;

    /**
     * @brief Draws the background under the menu.
     *
     * @param gameTime The elapsed time.
     */
    void Draw(const GameTime& gameTime) override;

protected:
    /**
     * @brief Backing out of this menu returns to the main menu.
     *
     * @param playerIndex Who cancelled.
     */
    void OnCancel(PlayerIndex playerIndex) override;

private:
    void NewGameMenuEntrySelected();
    void LoadGameMenuEntrySelected();

    std::optional<Texture2D> background_;
};

} // namespace Yacht
