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
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/TimeSpan.hpp"

#include "GameScreen.hpp"
#include "MenuEntry.hpp"

namespace GameStateManagement {

using Microsoft::Xna::Framework::Point;
using Microsoft::Xna::Framework::Input::Buttons;
using Microsoft::Xna::Framework::Input::Keys;
using Microsoft::Xna::Framework::Input::Touch::GestureSample;
using Microsoft::Xna::Framework::Input::Touch::GestureType;
using System::TimeSpan;

/**
 * @brief Base class for screens that contain a menu of options.
 *
 * The user can move up and down to select an entry, or cancel to back out of the screen.
 *
 * The original shares this class between Windows, Xbox and Windows Phone. Yacht selects the
 * phone branch; on desktop the platform delivers mouse presses as phone touches.
 */
class MenuScreen : public GameScreen {
public:
    /**
     * @brief Constructor.
     *
     * @param menuTitle The title drawn under the entries.
     */
    explicit MenuScreen(std::string menuTitle) : menuTitle_(std::move(menuTitle))
    {
#if defined(YACHT_WINDOWS_PHONE)
        setEnabledGesturesProperty(GestureType::Tap);
#endif

        setTransitionOnTimeProperty(TimeSpan::FromSeconds(0.5));
        setTransitionOffTimeProperty(TimeSpan::FromSeconds(0.5));
    }

    /**
     * @brief Responds to user input, changing the selected entry and accepting or cancelling.
     *
     * @param input This frame's input.
     */
    void HandleInput(InputState& input) override
    {
        PlayerIndex player = PlayerIndex::One;
        if (input.IsNewButtonPress(Buttons::Back, getControllingPlayerProperty(), player)) {
            OnCancel(player);
        }

#if defined(YACHT_WINDOWS)
        if (!menuEntries_.empty()) {
            if (input.IsMenuUp(getControllingPlayerProperty())) {
                if (--selectedEntry_ < 0) selectedEntry_ = static_cast<int>(menuEntries_.size()) - 1;
            } else if (input.IsMenuDown(getControllingPlayerProperty())) {
                if (++selectedEntry_ >= static_cast<int>(menuEntries_.size())) selectedEntry_ = 0;
            } else if (input.IsNewKeyPress(Keys::Enter, getControllingPlayerProperty(), player) ||
                       input.IsNewKeyPress(Keys::Space, getControllingPlayerProperty(), player)) {
                OnSelectEntry(selectedEntry_, player);
            }
        }

        const auto state = Microsoft::Xna::Framework::Input::Mouse::GetState();
        const Point clickLocation(state.getXProperty(), state.getYProperty());
        if (state.getLeftButtonProperty() == Microsoft::Xna::Framework::Input::ButtonState::Released) {
            if (isMouseDown_) {
                isMouseDown_ = false;
                for (std::size_t i = 0; i < menuEntries_.size(); ++i) {
                    if (menuEntries_[i]->getDestinationProperty().Contains(clickLocation))
                        OnSelectEntry(static_cast<int>(i), PlayerIndex::One);
                }
            }
        } else if (state.getLeftButtonProperty() ==
                   Microsoft::Xna::Framework::Input::ButtonState::Pressed) {
            isMouseDown_ = true;
            for (std::size_t i = 0; i < menuEntries_.size(); ++i) {
                if (menuEntries_[i]->getDestinationProperty().Contains(clickLocation))
                    selectedEntry_ = static_cast<int>(i);
            }
        }
#elif defined(YACHT_XBOX)
        if (!menuEntries_.empty()) {
            if (input.IsMenuUp(getControllingPlayerProperty())) {
                if (--selectedEntry_ < 0) selectedEntry_ = static_cast<int>(menuEntries_.size()) - 1;
            } else if (input.IsMenuDown(getControllingPlayerProperty())) {
                if (++selectedEntry_ >= static_cast<int>(menuEntries_.size())) selectedEntry_ = 0;
            } else if (input.IsNewButtonPress(Buttons::A, getControllingPlayerProperty(), player)) {
                OnSelectEntry(selectedEntry_, player);
            }
        }
#elif defined(YACHT_WINDOWS_PHONE)
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
#endif
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
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

        // Update each nested MenuEntry object.
        for (std::size_t i = 0; i < menuEntries_.size(); i++) {
            const bool isSelected = getIsActiveProperty() && (static_cast<int>(i) == selectedEntry_);
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
#if defined(YACHT_WINDOWS)
    bool isMouseDown_ = false;
#endif
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
