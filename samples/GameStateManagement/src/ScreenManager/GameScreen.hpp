// SPDX-License-Identifier: MS-PL
#pragma once

#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "System/Object.hpp"
#include "System/TimeSpan.hpp"

namespace GameStateManagement
{
    class InputState;
    class ScreenManager;

    /** @brief Describes the current transition state of a game screen. */
    enum class ScreenState { TransitionOn, Active, TransitionOff, Hidden };

    /** @brief Represents one updateable and drawable layer managed by ScreenManager. */
    class GameScreen : public System::Object
    {
    public:
        /** @brief Destroys the screen. */
        ~GameScreen() override = default;
        /** @brief Gets whether this screen is a popup. @return true for a popup screen. */
        [[nodiscard]] bool getIsPopupProperty() const;
        /** @brief Gets the transition-on duration. @return Transition-on duration. */
        [[nodiscard]] System::TimeSpan getTransitionOnTimeProperty() const;
        /** @brief Gets the transition-off duration. @return Transition-off duration. */
        [[nodiscard]] System::TimeSpan getTransitionOffTimeProperty() const;
        /** @brief Gets the transition position. @return Value from zero to one. */
        [[nodiscard]] float getTransitionPositionProperty() const;
        /** @brief Gets the transition alpha. @return Value from one to zero. */
        [[nodiscard]] float getTransitionAlphaProperty() const;
        /** @brief Gets the transition state. @return Current screen state. */
        [[nodiscard]] ScreenState getScreenStateProperty() const;
        /** @brief Gets whether the screen is exiting permanently. @return true while exiting. */
        [[nodiscard]] bool getIsExitingProperty() const;
        /** @brief Sets whether the screen is exiting permanently. @param value New exiting state. */
        void setIsExitingProperty(bool value);
        /** @brief Gets whether the screen can currently receive input. @return true when active. */
        [[nodiscard]] bool getIsActiveProperty() const;
        /** @brief Gets the owning screen manager. @return Owning manager. */
        [[nodiscard]] ScreenManager& getScreenManagerProperty() const;
        /** @brief Sets the owning screen manager. @param value Owning manager. */
        void setScreenManagerProperty(ScreenManager& value);
        /** @brief Gets the controlling player. @return Player index or no value for any player. */
        [[nodiscard]] std::optional<Microsoft::Xna::Framework::PlayerIndex>
        getControllingPlayerProperty() const;
        /** @brief Sets the controlling player. @param value Player index or no value. */
        void setControllingPlayerProperty(
            std::optional<Microsoft::Xna::Framework::PlayerIndex> value);
        /** @brief Gets gestures requested by this screen. @return Requested gesture mask. */
        [[nodiscard]] Microsoft::Xna::Framework::Input::Touch::GestureType
        getEnabledGesturesProperty() const;
        /** @brief Sets gestures requested by this screen. @param value Gesture mask. */
        void setEnabledGesturesProperty(
            Microsoft::Xna::Framework::Input::Touch::GestureType value);
        /** @brief Loads content owned by the screen. */
        virtual void LoadContent();
        /** @brief Unloads content owned by the screen. */
        virtual void UnloadContent();
        /** @brief Updates screen transition state. @param gameTime Timing. @param otherScreenHasFocus Focus state. @param coveredByOtherScreen Cover state. */
        virtual void Update(Microsoft::Xna::Framework::GameTime& gameTime,
                            bool otherScreenHasFocus, bool coveredByOtherScreen);
        /** @brief Handles input. @param input Current input snapshot. */
        virtual void HandleInput(InputState& input);
        /** @brief Draws the screen. @param gameTime Timing. */
        virtual void Draw(const Microsoft::Xna::Framework::GameTime& gameTime);
        /** @brief Begins exiting with the configured transition. */
        void ExitScreen();

        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] virtual const std::string& GetTypeName() const override = 0;

    protected:
        /** @brief Sets popup state. @param value New state. */
        void setIsPopupProperty(bool value);
        /** @brief Sets transition-on time. @param value New duration. */
        void setTransitionOnTimeProperty(System::TimeSpan value);
        /** @brief Sets transition-off time. @param value New duration. */
        void setTransitionOffTimeProperty(System::TimeSpan value);

    private:
        bool UpdateTransition(Microsoft::Xna::Framework::GameTime& gameTime,
                              System::TimeSpan time, int direction);

        bool isPopup_ = false;
        System::TimeSpan transitionOnTime_ = System::TimeSpan::Zero;
        System::TimeSpan transitionOffTime_ = System::TimeSpan::Zero;
        float transitionPosition_ = 1.0f;
        ScreenState screenState_ = ScreenState::TransitionOn;
        bool isExiting_ = false;
        bool otherScreenHasFocus_ = false;
        ScreenManager* screenManager_ = nullptr;
        std::optional<Microsoft::Xna::Framework::PlayerIndex> controllingPlayer_;
        Microsoft::Xna::Framework::Input::Touch::GestureType enabledGestures_ =
            Microsoft::Xna::Framework::Input::Touch::GestureType::None;
    };
}
