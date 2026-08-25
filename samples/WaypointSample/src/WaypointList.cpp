// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// WaypointList.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "WaypointList.hpp"

#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"

namespace Waypoint
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    void WaypointList::LoadContent(ContentManager& content)
    {
        waypointTexture = content.Load<Texture2D>("dot");
        waypointCenter =
            Vector2((float)(waypointTexture.getWidthProperty() / 2),
                    (float)(waypointTexture.getHeightProperty() / 2));
    }

    void WaypointList::Draw(SpriteBatch& spriteBatch) const
    {
        if (getCountProperty() == 1)
        {
            // If we only have a single waypoint in the list we don't have to
            // worry about creating a nice color gradient, so we just draw it
            // as completely red here
            spriteBatch.Begin();
            spriteBatch.Draw(waypointTexture, Peek(), std::nullopt, Color::Red,
                0.0f, waypointCenter, 1.0f, SpriteEffects::None, 0.0f);
            spriteBatch.End();
        }
        else if (getCountProperty() > 0)
        {
            // In this case we have more than one waypoint on the list, so we
            // want to fade smoothly from red for the first and blue for the
            // last so we can tell visually what order they're in
            float numberPoints = (float)getCountProperty() - 1;
            float i = 0;
            spriteBatch.Begin();
            for (const Vector2& location : *this)
            {
                // This creates a gradient between 0 for the first waypoint on
                // the list and 1 for the last, 0 creates a color that's
                // completely red and 1 creates a color that's completely blue
                spriteBatch.Draw(waypointTexture, location, std::nullopt,
                    Color(Vector4::Lerp(Color::Red.ToVector4(),
                    Color::Blue.ToVector4(), i / numberPoints)),
                    0.0f, waypointCenter, 1.0f, SpriteEffects::None, 0.0f);

                i++;
            }
            spriteBatch.End();
        }
    }
}
