// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// UIElement.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchLocation.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace RimLighting
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using SharpRuntime::String;

    /**
     * @brief Defines the base class for a drawable UI control.
     */
    class UIElement
    {
    public:
        /** @brief Releases the element. */
        virtual ~UIElement() = default;

        /**
         * @brief Gets the position of the UIElement.
         * @return The position, in screen pixels.
         */
        [[nodiscard]] Vector2 getPositionProperty() const;

        /**
         * @brief Sets the position of the UIElement.
         * @param value The position, in screen pixels.
         */
        void setPositionProperty(Vector2 value);

        /**
         * @brief Gets the size of the UIElement.
         * @return The size, in screen pixels.
         */
        [[nodiscard]] Vector2 getSizeProperty() const;

        /**
         * @brief Sets the size of the UIElement.
         * @param value The size, in screen pixels.
         */
        void setSizeProperty(Vector2 value);

        /**
         * @brief Gets the visibility of the UIElement.
         * @return True when the element draws and accepts input.
         */
        [[nodiscard]] bool getIsVisibleProperty() const;

        /**
         * @brief Sets the visibility of the UIElement.
         * @param value True to make the element draw and accept input.
         */
        void setIsVisibleProperty(bool value);

        /**
         * @brief Renders a UIElement, measuring first if necessary.
         * @param spriteBatch The batch the element draws its text through.
         */
        virtual void Draw(SpriteBatch& spriteBatch);

        /**
         * @brief Offers one touch location to the element.
         * @param loc The touch location to process.
         */
        virtual void HandleTouch(const Input::Touch::TouchLocation& loc);

    protected:
        /** @brief Constructs a visible element. */
        UIElement();

        /** Derived classes should implement this when they can be sized and formatted. */
        virtual void Measure() = 0;

        /**
         * A simple word-wrap algorithm that formats based on word-breaks. it's not completely
         * accurate with respect to kerning and spaces and doesn't handle localized text, but is
         * easy to read for sample use.
         */
        static String WordWrap(const String& input, int width, const SpriteFont& font);

        bool needsMeasure = false;
        Vector2 position;
        Vector2 size;

    private:
        bool isVisible;
    };
}
