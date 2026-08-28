// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Clickable.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Clickable.hpp"

#include "../Graphics3DSampleGame.hpp"

#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchPanel.hpp"

namespace Graphics3DSample
{
    using Microsoft::Xna::Framework::Input::Touch::TouchCollection;
    using Microsoft::Xna::Framework::Input::Touch::TouchPanel;

    Clickable::Clickable(Graphics3DSampleGame& game, const Rectangle& targetRectangle)
        : DrawableGameComponent(game), rectangle(targetRectangle)
    {
    }

    const std::string& Clickable::GetTypeName() const
    {
        static const std::string name = "Graphics3DSample.Clickable";
        return name;
    }

    Graphics3DSampleGame& Clickable::getGameProperty() const
    {
        return static_cast<Graphics3DSampleGame&>(DrawableGameComponent::getGameProperty());
    }

    void Clickable::HandleInput()
    {
        wasTouching = isTouching;
        isTouching = false;

        TouchCollection touches = TouchPanel::GetState();

        if (touches.getCountProperty() > 0)
        {
            const auto& touch = touches[0];

            Rectangle touchRect(static_cast<int>(touch.getPositionProperty().X) - 5,
                                static_cast<int>(touch.getPositionProperty().Y) - 5,
                                10, 10);

            if (rectangle.Intersects(touchRect))
            {
                isTouching = true;
            }
        }
    }
}
