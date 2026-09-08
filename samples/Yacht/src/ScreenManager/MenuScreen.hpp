#pragma once

// MenuScreen.hpp -- C++ port of Yacht/ScreenManager/MenuScreen.cs.

#include <cmath>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/TimeSpan.hpp"

#include "../GameScreen.hpp"
#include "MenuEntry.hpp"

namespace GameStateManagement {

using Microsoft::Xna::Framework::Point;
using Microsoft::Xna::Framework::Input::Buttons;
using Microsoft::Xna::Framework::Input::Touch::GestureSample;
using Microsoft::Xna::Framework::Input::Touch::GestureType;
using System::TimeSpan;

/**
 * @brief Base class for screens that contain a menu of options.
 *
 * The user can move up and down to select an entry, or cancel to back out of the screen.
 *
 * @note **Only the touch branch of the original is ported, because only it is this product's.**
 * MenuScreen.cs carries three input paths behind `#if WINDOWS`, `#elif XBOX` and `#elif
 * WINDOWS_PHONE`; Yacht is the phone product and compiles the last. The other two are the same
 * class serving other products in the Game State Management sample, and compiling them here
 * would give this game keyboard and mouse navigation it does not have. The mouse still works,
 * because the platform delivers it as touch -- see TouchPanel::setMouseTouchEmulationEnabledEXT
 * in YachtGame.
 */
class MenuScreen : public Yacht::GameScreen {
public:
    /**
     * @brief Constructor.
     *
     * @param menuTitle The title drawn under the entries.
     */
    explicit MenuScreen(std::string menuTitle) : menuTitle_(std::move(menuTitle))
    {
        setEnabledGestures(GestureType::Tap);

        setTransitionOnTime(TimeSpan::FromSeconds(0.5));
        setTransitionOffTime(TimeSpan::FromSeconds(0.5));
    }

    /**
     * @brief Responds to user input, changing the selected entry and accepting or cancelling.
     *
     * @param input This frame's input.
     */
    void HandleInput(Yacht::InputState& input) override
    {
        PlayerIndex player = PlayerIndex::One;
        if (input.IsNewButtonPress(Buttons::Back, ControllingPlayer(), player)) {
            OnCancel(player);
        }

        for (const GestureSample& gesture : input.Gestures) {
            if (gesture.getGestureTypeProperty() == GestureType::Tap) {
                const Point tapLocation(static_cast<int>(gesture.getPositionProperty().X),
                                        static_cast<int>(gesture.getPositionProperty().Y));

                for (std::size_t i = 0; i < menuEntries_.size(); i++) {
                    if (menuEntries_[i]->getDestinationProperty().Contains(tapLocation)) {
                        OnSelectEntry(static_cast<int>(i), PlayerIndex::One);
                    }
                }
            }
        }
    }

    /**
     * @brief Updates the menu.
     *
     * @param gameTime             The elapsed time.
     * @param otherScreenHasFocus  Whether another screen has focus.
     * @param coveredByOtherScreen Whether another screen covers this one.
     */
    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override
    {
        Yacht::GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

        // Update each nested MenuEntry object.
        for (std::size_t i = 0; i < menuEntries_.size(); i++) {
            const bool isSelected = IsActive() && (static_cast<int>(i) == selectedEntry_);
            menuEntries_[i]->Update(*this, isSelected, gameTime);
        }
    }

    /**
     * @brief Draws the menu.
     *
     * @param gameTime The elapsed time.
     */
    void Draw(const GameTime& gameTime) override;

    /** @brief Loads content, taking the safe area the entries are laid out in. */
    void LoadContent() override;

    /** @brief Places every entry, centred, one under another. */
    void UpdateMenuEntryDestination();

protected:
    /**
     * @brief Gets the list of menu entries, so derived classes can add or change the menu.
     *
     * @return The entries.
     */
    [[nodiscard]] std::vector<std::shared_ptr<MenuEntry>>& MenuEntries() { return menuEntries_; }

    /**
     * @brief The area a tap on an entry is accepted in.
     *
     * Wider than the entry itself: a finger is not a cursor, so the row is padded above and
     * below and spans the screen.
     *
     * @param entry The entry to bound.
     * @return The rectangle a tap must land in.
     */
    [[nodiscard]] virtual Rectangle GetMenuEntryHitBounds(const MenuEntry& entry) const;

    /**
     * @brief Handler for when the user has chosen a menu entry.
     *
     * @param entryIndex  Which entry.
     * @param playerIndex Who chose it.
     */
    virtual void OnSelectEntry(int entryIndex, PlayerIndex playerIndex)
    {
        menuEntries_[static_cast<std::size_t>(entryIndex)]->OnSelectEntry(playerIndex);
    }

    /**
     * @brief Handler for when the user has cancelled the menu.
     *
     * @param playerIndex Who cancelled.
     */
    virtual void OnCancel(PlayerIndex playerIndex)
    {
        (void)playerIndex;
        ExitScreen();
    }

    /**
     * @brief Helper overload makes it easy to use OnCancel as a MenuEntry event handler.
     *
     * @param sender The entry.
     * @param e      Who selected it.
     */
    void OnCancel(System::Object* sender, const PlayerIndexEventArgs& e)
    {
        (void)sender;
        OnCancel(e.getPlayerIndexProperty());
    }

    /** @brief Allows the screen the chance to position the menu entries as it transitions. */
    virtual void UpdateMenuEntryLocations();

private:
    static constexpr int menuEntryPadding_ = 35;

    std::vector<std::shared_ptr<MenuEntry>> menuEntries_;
    int selectedEntry_ = 0;
    std::string menuTitle_;
    Rectangle bounds_;
};

} // namespace GameStateManagement

namespace Yacht {

// The menu classes belong to the Game State Management sample's namespace, as they do in the
// original; the screens that derive from them are Yacht's own and name them unqualified.
using GameStateManagement::MenuEntry;
using GameStateManagement::MenuScreen;
using GameStateManagement::PlayerIndexEventArgs;

} // namespace Yacht
