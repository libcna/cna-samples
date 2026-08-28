// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Checkbox.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <string>

#include "Clickable.hpp"

#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Graphics3DSample
{
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /**
     * @brief A game component, inherits to Clickable.
     *
     * Has associated On and Off content. Has a state of IsChecked that is switched by click.
     * Draws content according to state.
     */
    class Checkbox : public Clickable
    {
    private:
        const std::string asset;
        // XNA's Texture2D is a reference type, so the original's field is null until LoadContent
        // assigns it; std::optional is this port's established stand-in.
        std::optional<Texture2D> textureOn;
        bool isChecked;

    public:
        /**
         * @brief Gets the checkbox's current state.
         * @return True when the checkbox is checked.
         */
        [[nodiscard]] bool getIsCheckedProperty() const { return isChecked; }

        /**
         * @brief Constructor.
         *
         * @param game The Game object.
         * @param textureName Texture name.
         * @param targetRectangle Position of the component on the screen.
         * @param isChecked Initial state of the checkbox.
         */
        Checkbox(Graphics3DSampleGame& game, const std::string& textureName,
                 const Rectangle& targetRectangle, bool isChecked);

        /**
         * @brief Returns the fully qualified logical type name of this component.
         * @return "Graphics3DSample.Checkbox".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Load the texture. */
        void LoadContent() override;

    public:
        /**
         * @brief Allows the game component to update itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Update(GameTime& gameTime) override;

        /**
         * @brief Allows the game component to draw itself.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;
    };
}
