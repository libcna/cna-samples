// SPDX-License-Identifier: MS-PL
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureSample.hpp"
#include "System/Object.hpp"

namespace DynamicMenu::Controls
{
    /** @brief Interface implemented by every dynamic-menu control. */
    class IControl : public System::Object
    {
    public:
        /** @brief Destroys the interface. */
        ~IControl() override = default;
        /** @brief Gets the control width. @return Width in pixels. */
        [[nodiscard]] virtual int getWidthProperty() const = 0;
        /** @brief Sets the control width. @param value Width in pixels. */
        virtual void setWidthProperty(int value) = 0;
        /** @brief Gets the control height. @return Height in pixels. */
        [[nodiscard]] virtual int getHeightProperty() const = 0;
        /** @brief Sets the control height. @param value Height in pixels. */
        virtual void setHeightProperty(int value) = 0;
        /** @brief Gets the top coordinate. @return Parent-relative coordinate. */
        [[nodiscard]] virtual int getTopProperty() const = 0;
        /** @brief Sets the top coordinate. @param value Parent-relative coordinate. */
        virtual void setTopProperty(int value) = 0;
        /** @brief Gets the left coordinate. @return Parent-relative coordinate. */
        [[nodiscard]] virtual int getLeftProperty() const = 0;
        /** @brief Sets the left coordinate. @param value Parent-relative coordinate. */
        virtual void setLeftProperty(int value) = 0;
        /** @brief Gets the bottom coordinate. @return Parent-relative coordinate. */
        [[nodiscard]] virtual int getBottomProperty() const = 0;
        /** @brief Gets the right coordinate. @return Parent-relative coordinate. */
        [[nodiscard]] virtual int getRightProperty() const = 0;
        /** @brief Gets the control name. @return The name. */
        [[nodiscard]] virtual const std::string& getNameProperty() const = 0;
        /** @brief Sets the control name. @param value The name. */
        virtual void setNameProperty(std::string value) = 0;
        /** @brief Gets the control hue. @return The hue. */
        [[nodiscard]] virtual Microsoft::Xna::Framework::Color getHueProperty() const = 0;
        /** @brief Sets the control hue. @param value The hue. */
        virtual void setHueProperty(Microsoft::Xna::Framework::Color value) = 0;
        /** @brief Gets the parent control. @return Non-owning parent pointer, or null. */
        [[nodiscard]] virtual IControl* getParentProperty() const = 0;
        /** @brief Sets the parent control. @param value Non-owning parent pointer. */
        virtual void setParentProperty(IControl* value) = 0;
        /** @brief Gets the absolute top-left position. @return Screen-space point. */
        [[nodiscard]] virtual Microsoft::Xna::Framework::Point GetAbsoluteTopLeft() const = 0;
        /**
         * @brief Updates the control.
         * @param gameTime Current game time.
         * @param gestures Gestures collected for the frame.
         */
        virtual void Update(
            const Microsoft::Xna::Framework::GameTime& gameTime,
            const std::vector<Microsoft::Xna::Framework::Input::Touch::GestureSample>& gestures) = 0;
    };
}
