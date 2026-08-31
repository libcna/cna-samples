// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"
#include "System/Object.hpp"

namespace DynamicMenu::Controls { class IControl; }

namespace DynamicMenu::Transitions
{
    /** @brief Applies animated position, size, and color effects to controls. */
    class Transition : public System::Object
    {
    public:
        /** @brief Occurs when the transition is complete. */
        System::EventHandler<System::EventArgs> TransitionComplete;

        /**
         * @brief Constructs a transition from optional start and end values.
         * @param startPosition Starting position.
         * @param endPosition Ending position.
         * @param startSize Starting size.
         * @param endSize Ending size.
         * @param startColor Starting color.
         * @param endColor Ending color.
         */
        Transition(
            std::optional<Microsoft::Xna::Framework::Point> startPosition,
            std::optional<Microsoft::Xna::Framework::Point> endPosition,
            std::optional<Microsoft::Xna::Framework::Point> startSize,
            std::optional<Microsoft::Xna::Framework::Point> endSize,
            std::optional<Microsoft::Xna::Framework::Color> startColor,
            std::optional<Microsoft::Xna::Framework::Color> endColor);

        /** @brief Gets the target control. @return Non-owning target pointer. */
        [[nodiscard]] DynamicMenu::Controls::IControl* getControlProperty() const;
        /** @brief Sets the target control. @param value Non-owning target pointer. */
        void setControlProperty(DynamicMenu::Controls::IControl* value);
        /** @brief Gets the start position. @return Start position. */
        [[nodiscard]] Microsoft::Xna::Framework::Point getStartPositionProperty() const;
        /** @brief Sets the start position. @param value Start position. */
        void setStartPositionProperty(Microsoft::Xna::Framework::Point value);
        /** @brief Gets the end position. @return End position. */
        [[nodiscard]] Microsoft::Xna::Framework::Point getEndPositionProperty() const;
        /** @brief Sets the end position. @param value End position. */
        void setEndPositionProperty(Microsoft::Xna::Framework::Point value);
        /** @brief Gets the start size. @return Start size. */
        [[nodiscard]] Microsoft::Xna::Framework::Point getStartSizeProperty() const;
        /** @brief Sets the start size. @param value Start size. */
        void setStartSizeProperty(Microsoft::Xna::Framework::Point value);
        /** @brief Gets the end size. @return End size. */
        [[nodiscard]] Microsoft::Xna::Framework::Point getEndSizeProperty() const;
        /** @brief Sets the end size. @param value End size. */
        void setEndSizeProperty(Microsoft::Xna::Framework::Point value);
        /** @brief Gets the start color. @return Start color. */
        [[nodiscard]] Microsoft::Xna::Framework::Color getStartColorProperty() const;
        /** @brief Sets the start color. @param value Start color. */
        void setStartColorProperty(Microsoft::Xna::Framework::Color value);
        /** @brief Gets the end color. @return End color. */
        [[nodiscard]] Microsoft::Xna::Framework::Color getEndColorProperty() const;
        /** @brief Sets the end color. @param value End color. */
        void setEndColorProperty(Microsoft::Xna::Framework::Color value);
        /** @brief Gets duration in seconds. @return Duration. */
        [[nodiscard]] float getTransitionLengthProperty() const;
        /** @brief Sets duration in seconds. @param value Duration. */
        void setTransitionLengthProperty(float value);
        /** @brief Gets whether the transition is active. @return Active state. */
        [[nodiscard]] bool getTransitionActiveProperty() const;

        /** @brief Begins applying the transition. */
        void StartTranstion();
        /** @brief Advances the transition. @param gameTime Current game time. */
        void Update(const Microsoft::Xna::Framework::GameTime& gameTime);

        /** @brief Creates a fade-in transition. @param control Target. @param length Optional duration. @return Transition. */
        static std::shared_ptr<Transition> CreateFadeIn(
            DynamicMenu::Controls::IControl& control, std::optional<float> length);
        /** @brief Creates a fade-out transition. @param control Target. @param length Optional duration. @return Transition. */
        static std::shared_ptr<Transition> CreateFadeOut(
            DynamicMenu::Controls::IControl& control, std::optional<float> length);
        /** @brief Creates a fly-in transition. @param control Target. @param startPos Start. @param length Optional duration. @return Transition. */
        static std::shared_ptr<Transition> CreateFlyIn(
            DynamicMenu::Controls::IControl& control,
            Microsoft::Xna::Framework::Point startPos,
            std::optional<float> length);
        /** @brief Creates a fly-out transition. @param control Target. @param endPos End. @param length Optional duration. @return Transition. */
        static std::shared_ptr<Transition> CreateFlyOut(
            DynamicMenu::Controls::IControl& control,
            Microsoft::Xna::Framework::Point endPos,
            std::optional<float> length);

        /** @brief Gets the fully-qualified logical type name. @return Type name. */
        [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        bool startPositionSet_ = false;
        bool endPositionSet_ = false;
        bool startSizeSet_ = false;
        bool endSizeSet_ = false;
        bool startHueSet_ = false;
        bool endHueSet_ = false;
        Microsoft::Xna::Framework::Point startPosition_;
        Microsoft::Xna::Framework::Point endPosition_;
        Microsoft::Xna::Framework::Point startSize_;
        Microsoft::Xna::Framework::Point endSize_;
        Microsoft::Xna::Framework::Color startHue_;
        Microsoft::Xna::Framework::Color endHue_;
        DynamicMenu::Controls::IControl* control_ = nullptr;
        float transitionLength_ = 1.0f;
        bool transitionActive_ = false;
        double transitionStartTime_ = 0.0;
    };
}
