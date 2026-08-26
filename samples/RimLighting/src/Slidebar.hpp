// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Slidebar.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>

#include "UIElement.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"

namespace RimLighting
{
    /**
     * @brief A Slidebar is a UI Element (control) that has a string of text and a bar whose
     *        length can be changed by dragging on it.
     */
    class Slidebar : public UIElement
    {
    public:
        /**
         * @brief Constructs a slidebar over the given value range.
         *
         * @param game The game whose ContentManager the bar texture is loaded from.
         * @param font The font the label is measured and drawn with.
         * @param min The minimum value the bar can represent.
         * @param max The maximum value the bar can represent.
         */
        Slidebar(Microsoft::Xna::Framework::Game& game, const SpriteFont& font, float min, float max);

        /** @brief Color of the control. */
        Microsoft::Xna::Framework::Color Color = Microsoft::Xna::Framework::Color::White;

        /**
         * @brief Gets the measured size of the control's text.
         * @return The text size, in pixels.
         */
        [[nodiscard]] Vector2 getTextSizeProperty() const;

        /**
         * @brief Gets the text of the control to show on screen.
         * @return The label text.
         */
        [[nodiscard]] String getTextProperty() const;

        /**
         * @brief Sets the text of the control to show on screen.
         * @param value The label text.
         */
        void setTextProperty(const String& value);

        /** @brief The minimum possible value represented by this slidebar. */
        float MinValue;

        /** @brief The maximum possible value represented by this slidebar. */
        float MaxValue;

        /**
         * @brief Gets the current value of the slidebar.
         * @return The current value.
         */
        [[nodiscard]] float getValueProperty() const;

        /**
         * @brief Sets the current value of the slidebar, raising OnValueChanged.
         * @param value The new value.
         */
        void setValueProperty(float value);

        /** @brief Is the user currently dragging on this slidebar? */
        bool IsDragging = false;

        /**
         * @brief Raised when Value changes, whether set directly or dragged.
         *
         * The original declares its own `delegate void ValueChangedHandler(object sender)`,
         * which carries no arguments beyond the sender; this is that delegate expressed through
         * the project-wide event type, whose EventArgs is unused.
         */
        System::EventHandler<System::EventArgs> OnValueChanged;

        /**
         * @brief Sets position and size of the bar.
         *
         * @param offsetX The relative X coordinate to the Text.
         * @param offsetY The relative Y coordinate to the Text.
         * @param maxwidth The max width of the bar in pixels.
         * @param height The height of the bar in pixels.
         */
        void SetBarOffsetSize(float offsetX, float offsetY, float maxwidth, float height);

        /**
         * @brief Sets the floating number range that this slidebar could represent.
         *
         * @param min The minimum value.
         * @param max The maximum value.
         */
        void SetRange(float min, float max);

        /**
         * @brief Handle the touch input and update the bar if necessary.
         * @param loc The touch location to process.
         */
        void HandleTouch(const Input::Touch::TouchLocation& loc) override;

        /**
         * @brief Renders the control to screen.
         * @param spriteBatch The batch the control draws through.
         */
        void Draw(SpriteBatch& spriteBatch) override;

    protected:
        void Measure() override;

        /** Load the texture for rendering the bar if it is not already loaded. */
        void LoadContent();

        Microsoft::Xna::Framework::Game* game = nullptr;
        const SpriteFont* spriteFont = nullptr;

    private:
        Vector2 textSize;
        String sliderText;

        // The relative position of the bar to the text
        Vector2 offsetBar;

        // The maximum extent of the bar
        Vector2 sizeBar;

        // The current length of the bar which is caculated from current Value together with
        // MinValue and MaxValue
        float currentLength = 0;

        float valueInt = 0;

        // The first position of the touch since the drag began
        Vector2 lastPressPosition;

        // Plain texture used for drawing the bar. The original holds it in a `static` field
        // shared by every Slidebar; a function-local static here keeps that one-load-per-process
        // behavior without a mutable namespace-scope global.
        static std::optional<Texture2D>& BlankTexture();
    };
}
