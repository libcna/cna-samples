// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Button.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <memory>

#include "UIElement.hpp"

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"

namespace RimLighting
{
    /**
     * @brief A Button is a UI Element that has an up and down state, an event handler, and a
     *        string of text.
     */
    class Button : public UIElement
    {
    public:
        /**
         * @brief Creates a new button object.
         *
         * @param device The device the button's own BasicEffect is created on.
         * @param font The font the button's text is measured and drawn with.
         * @param text The text displayed on the button.
         */
        Button(GraphicsDevice& device, const SpriteFont& font, const String& text);

        /**
         * @brief Gets the text displayed on the button.
         * @return The button's text.
         */
        [[nodiscard]] String getTextProperty() const;

        /**
         * @brief Sets the text displayed on the button.
         * @param value The button's text.
         */
        void setTextProperty(const String& value);

        /**
         * @brief OnClick fires when the button is pressed and released.
         *
         * The original declares its own `delegate void ClickEventHandler(object sender)`, which
         * carries no arguments beyond the sender; this is that delegate expressed through the
         * project-wide event type, whose EventArgs is unused.
         */
        System::EventHandler<System::EventArgs> OnClick;

        /**
         * @brief check to see if the button was pressed or released.
         * @param loc The touch location to process.
         */
        void HandleTouch(const Input::Touch::TouchLocation& loc) override;

        /**
         * @brief Renders the button.
         *
         * This function mixes spritebatch and 3D drawing so it should not be called from within
         * a SpriteBatch.Begin/End block.
         *
         * @param spriteBatch The batch the button's text is drawn through.
         */
        void Draw(SpriteBatch& spriteBatch) override;

    protected:
        /** Check to see if the button's rectangle contains the given point. */
        [[nodiscard]] bool HitTest(Vector2 point) const;

        /** Called when the button's attributes are 'dirty' and the visuals need to be updated. */
        void Measure() override;

        const SpriteFont* buttonFont = nullptr;
        std::array<VertexPositionColor, 6> verts{};
        Vector2 textOffset;
        int pressId = 0;
        String buttonText;

    private:
        std::unique_ptr<BasicEffect> effect;
    };
}
