#pragma once

// GameScreen.hpp -- C++ port of Yacht/ScreenManager/GameScreen.cs.

#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "System/IO/Stream.hpp"
#include "System/Object.hpp"
#include "System/TimeSpan.hpp"

#include "InputState.hpp"

namespace GameStateManagement {

using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Input::Touch::GestureType;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;
using System::TimeSpan;

class ScreenManager;

/** @brief Enum describes the screen transition state. */
enum class ScreenState {
    /** @brief The screen is sliding in. */
    TransitionOn,
    /** @brief The screen is fully in place and taking input. */
    Active,
    /** @brief The screen is sliding out. */
    TransitionOff,
    /** @brief The screen is behind another and is not drawn. */
    Hidden
};

/**
 * @brief A screen is a single layer that has update and draw logic, and which can be combined
 *        with other layers to build up a complex menu system.
 *
 * For instance the main menu, the options menu, the "are you sure you want to quit" message box,
 * and the main game itself are all implemented as screens.
 */
class GameScreen : public System::Object {
public:
    /**
     * @brief The screen's own type name.
     *
     * C++ cannot ask an object what type it is, and the screen list is written by name, so
     * every screen states its own -- the same explicit table the content readers need for the
     * same reason.
     *
     * @return The name.
     */
    [[nodiscard]] const std::string& GetTypeName() const override = 0;

    /** @brief Destroys the screen. */
    virtual ~GameScreen() = default;

    /**
     * @brief Normally when one screen is brought up over the top of another, the first screen
     *        will transition off to make room for the new one.
     *
     * This property indicates whether the screen is only a small popup, in which case screens
     * underneath it do not need to bother transitioning off.
     *
     * @return True when this screen is a popup.
     */
    [[nodiscard]] bool getIsPopupProperty() const { return isPopup_; }

    /**
     * @brief Indicates how long the screen takes to transition on when it is activated.
     *
     * @return The transition-on time.
     */
    [[nodiscard]] TimeSpan getTransitionOnTimeProperty() const { return transitionOnTime_; }

    /**
     * @brief Indicates how long the screen takes to transition off when it is deactivated.
     *
     * @return The transition-off time.
     */
    [[nodiscard]] TimeSpan getTransitionOffTimeProperty() const { return transitionOffTime_; }

    /**
     * @brief Gets the current position of the screen transition.
     *
     * Ranges from zero (fully active, no transition) to one (transitioned fully off to nothing).
     *
     * @return The transition position.
     */
    [[nodiscard]] float getTransitionPositionProperty() const { return transitionPosition_; }

    /**
     * @brief Gets the current alpha of the screen transition.
     *
     * Ranges from one (fully active, no transition) to zero (transitioned fully off to nothing).
     *
     * @return The transition alpha.
     */
    [[nodiscard]] float getTransitionAlphaProperty() const
    {
        return 1.0f - getTransitionPositionProperty();
    }

    /**
     * @brief Gets the current screen transition state.
     *
     * @return The state.
     */
    [[nodiscard]] ScreenState getScreenStateProperty() const { return screenState_; }

    /**
     * @brief There are two possible reasons why a screen might be transitioning off.
     *
     * It could be temporarily going away to make room for another screen that is on top of it,
     * or it could be going away for good. This property indicates whether the screen is exiting
     * for real: if set, the screen will automatically remove itself as soon as the transition
     * finishes.
     *
     * @return True when the screen is going away for good.
     */
    [[nodiscard]] bool getIsExitingProperty() const { return isExiting_; }

    /**
     * @brief Sets whether the screen is going away for good.
     *
     * @param value True to remove the screen once it has transitioned off.
     */
    void setIsExitingProperty(bool value) { isExiting_ = value; }

    /**
     * @brief Checks whether this screen is active and can respond to user input.
     *
     * @return True when the screen has focus and is on or coming on.
     */
    [[nodiscard]] bool getIsActiveProperty() const
    {
        return !otherScreenHasFocus_ && (screenState_ == ScreenState::TransitionOn ||
                                         screenState_ == ScreenState::Active);
    }

    /**
     * @brief Gets the manager that this screen belongs to.
     *
     * @return The manager.
     */
    [[nodiscard]] ScreenManager* getScreenManagerProperty() const { return screenManager_; }

    /**
     * @brief Sets the manager that this screen belongs to.
     *
     * @param value The manager.
     */
    void setScreenManagerProperty(ScreenManager* value) { screenManager_ = value; }

    /**
     * @brief Gets the index of the player who is currently controlling this screen.
     *
     * This is used to lock the game to a specific player profile. The value is null when it is
     * accepting input from any player.
     *
     * @return The controlling player, or nothing.
     */
    [[nodiscard]] const std::optional<PlayerIndex>& getControllingPlayerProperty() const
    {
        return controllingPlayer_;
    }

    /**
     * @brief Sets the index of the player who is currently controlling this screen.
     *
     * @param value The controlling player, or nothing to accept any player.
     */
    void setControllingPlayerProperty(const std::optional<PlayerIndex>& value)
    {
        controllingPlayer_ = value;
    }

    /**
     * @brief Gets the gestures the screen is interested in.
     *
     * @return The enabled gestures.
     */
    [[nodiscard]] GestureType getEnabledGesturesProperty() const { return enabledGestures_; }

    /**
     * @brief Sets the gestures the screen is interested in.
     *
     * Screens should be as specific as possible with gestures to increase the accuracy of the
     * gesture engine. For example, most menus only need Tap or perhaps Tap and VerticalDrag to
     * operate. These gestures are handled by the ScreenManager when screens change and all
     * gestures are placed in the InputState passed to the HandleInput method.
     *
     * @param value The gestures to enable.
     */
    void setEnabledGesturesProperty(GestureType value)
    {
        enabledGestures_ = value;

        // The screen manager handles this during screen changes, but if this screen is
        // active and the gesture types are changing, we have to set the TouchPanel's
        // gesture types now.
        if (getScreenStateProperty() == ScreenState::Active) {
            TouchPanel::setEnabledGesturesProperty(value);
        }
    }

    /**
     * @brief Gets whether the screen is serialized when the game is put down.
     *
     * @return True when this screen is written into the screen list.
     */
    [[nodiscard]] bool getIsSerializableProperty() const { return isSerializable_; }

    /**
     * @brief Sets whether the screen is serialized when the game is put down.
     *
     * @param value True to include this screen in the screen list.
     */
    void setIsSerializableProperty(bool value) { isSerializable_ = value; }

    /** @brief Load graphics content for the screen. */
    virtual void LoadContent() {}

    /** @brief Unload content for the screen. */
    virtual void UnloadContent() {}

    /**
     * @brief Allows the screen to run logic, such as updating the transition position.
     *
     * Unlike HandleInput, this method is called regardless of whether the screen is active,
     * hidden, or in the middle of a transition.
     *
     * @param gameTime             The elapsed time.
     * @param otherScreenHasFocus  Whether another screen has focus.
     * @param coveredByOtherScreen Whether another screen covers this one.
     */
    virtual void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen);

    /**
     * @brief Allows the screen to handle user input.
     *
     * Unlike Update, this method is only called when the screen is active, and not when some
     * other screen has taken the focus.
     *
     * @param input This frame's input.
     */
    virtual void HandleInput(InputState& input) { (void)input; }

    /**
     * @brief This is called when the screen should draw itself.
     *
     * @param gameTime The elapsed time.
     */
    virtual void Draw(const GameTime& gameTime) { (void)gameTime; }

    /**
     * @brief Writes the screen's own state, for a screen that has any.
     *
     * @param stream Where to write it.
     */
    virtual void Serialize(System::IO::Stream& stream) { (void)stream; }

    /**
     * @brief Reads back what Serialize wrote.
     *
     * @param stream Where to read it from.
     */
    virtual void Deserialize(System::IO::Stream& stream) { (void)stream; }

    /**
     * @brief Tells the screen to go away.
     *
     * Unlike ScreenManager.RemoveScreen, which instantly kills the screen, this method
     * respects the transition timings and will give the screen a chance to gradually
     * transition off.
     */
    void ExitScreen();

    /**
     * @brief A helper method which loads assets using the screen manager's content manager.
     *
     * @tparam T        The asset's type.
     * @param assetName The asset to load.
     * @return The asset.
     */
    template <typename T>
    T Load(const std::string& assetName);

protected:
    /**
     * @brief Marks this screen as a popup, so screens underneath do not transition off.
     *
     * @param value True for a popup.
     */
    void setIsPopupProperty(bool value) { isPopup_ = value; }

    /**
     * @brief Sets how long the screen takes to transition on.
     *
     * @param value The time.
     */
    void setTransitionOnTimeProperty(TimeSpan value) { transitionOnTime_ = value; }

    /**
     * @brief Sets how long the screen takes to transition off.
     *
     * @param value The time.
     */
    void setTransitionOffTimeProperty(TimeSpan value) { transitionOffTime_ = value; }

private:
    // Helper for updating the screen transition position.
    bool UpdateTransition(const GameTime& gameTime, TimeSpan time, int direction)
    {
        // How much should we move by?
        float transitionDelta = 0.0f;

        if (time == TimeSpan::Zero) {
            transitionDelta = 1.0f;
        } else {
            transitionDelta =
                static_cast<float>(gameTime.getElapsedGameTimeProperty()
                                       .getTotalMillisecondsProperty() /
                                   time.getTotalMillisecondsProperty());
        }

        // Update the transition position.
        transitionPosition_ += transitionDelta * static_cast<float>(direction);

        // Did we reach the end of the transition?
        if ((direction < 0 && transitionPosition_ <= 0) ||
            (direction > 0 && transitionPosition_ >= 1)) {
            transitionPosition_ = MathHelper::Clamp(transitionPosition_, 0.0f, 1.0f);
            return false;
        }

        // Otherwise we are still busy transitioning.
        return true;
    }

    bool isPopup_ = false;
    TimeSpan transitionOnTime_ = TimeSpan::Zero;
    TimeSpan transitionOffTime_ = TimeSpan::Zero;
    float transitionPosition_ = 1.0f;
    ScreenState screenState_ = ScreenState::TransitionOn;
    bool isExiting_ = false;
    bool otherScreenHasFocus_ = false;
    ScreenManager* screenManager_ = nullptr;
    std::optional<PlayerIndex> controllingPlayer_;
    GestureType enabledGestures_ = GestureType::None;
    bool isSerializable_ = true;
};

} // namespace GameStateManagement

namespace Yacht {

using GameStateManagement::GameScreen;
using GameStateManagement::ScreenState;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Input::Touch::GestureType;
using Microsoft::Xna::Framework::Input::Touch::TouchPanel;
using System::TimeSpan;

} // namespace Yacht
