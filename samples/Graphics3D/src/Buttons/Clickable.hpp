// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Clickable.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"

namespace Graphics3DSample
{
    using Microsoft::Xna::Framework::DrawableGameComponent;
    using Microsoft::Xna::Framework::Rectangle;

    class Graphics3DSampleGame;

    /**
     * @brief A game component.
     *
     * Has an associated rectangle. Accepts touch and click inside the rectangle. Has a state of
     * IsTouching and IsClicked.
     */
    class Clickable : public DrawableGameComponent
    {
    private:
        const Rectangle rectangle;
        bool wasTouching = false;
        bool isTouching = false;

    public:
        /**
         * @brief Gets whether a touch point currently lies inside this component's rectangle.
         * @return True while the component is being touched.
         */
        [[nodiscard]] bool getIsTouchingProperty() const { return isTouching; }

        /**
         * @brief Gets whether the touch was released over this component in the current frame.
         * @return True on the frame the touch ends inside the rectangle.
         */
        [[nodiscard]] bool getIsClickedProperty() const { return (wasTouching == true) && (isTouching == false); }

    protected:
        /**
         * @brief Gets the position of the component on the screen.
         * @return The component's rectangle.
         */
        [[nodiscard]] const Rectangle& getRectangleProperty() const { return rectangle; }

        /**
         * @brief Gets the owning game, typed as this sample's own Game subclass.
         * @return The Graphics3DSampleGame that owns this component.
         */
        [[nodiscard]] Graphics3DSampleGame& getGameProperty() const;

    public:
        /**
         * @brief Constructor.
         *
         * @param game The Game object.
         * @param targetRectangle Position of the component on the screen.
         */
        Clickable(Graphics3DSampleGame& game, const Rectangle& targetRectangle);

        /**
         * @brief Returns the fully qualified logical type name of this component.
         * @return "Graphics3DSample.Clickable".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Handles Input. */
        void HandleInput();
    };
}
