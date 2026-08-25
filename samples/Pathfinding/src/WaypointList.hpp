// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// WaypointList.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Collections/Generic/Queue.hpp"

namespace Pathfinding
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using Microsoft::Xna::Framework::Content::ContentManager;

    /**
     * @brief WaypointList is a drawable List of screen locations.
     */
    class WaypointList : public System::Collections::Generic::Queue<Vector2>
    {
        /** Scales the draw size of the search nodes */
        static constexpr float waypointNodeDrawScale = 0.75f;

        float scale = 1.0f;

        // Draw data
        Texture2D waypointTexture;
        Vector2 waypointCenter;

    public:
        /**
         * @brief Gets the draw scale of the waypoint dots.
         * @return The scale already multiplied by the node draw scale.
         */
        [[nodiscard]] float getScaleProperty() const { return scale; }

        /**
         * @brief Sets the draw scale of the waypoint dots.
         * @param value Tile scale; the node draw scale is applied to it.
         */
        void setScaleProperty(float value) { scale = value * waypointNodeDrawScale; }

        /**
         * @brief Load the WaypointList's texture resources.
         * @param content The content manager to load from.
         */
        void LoadContent(ContentManager& content)
        {
            waypointTexture = content.Load<Texture2D>("dot");
            waypointCenter = Vector2((float)(waypointTexture.getWidthProperty() / 2),
                                     (float)(waypointTexture.getHeightProperty() / 2));
        }

        /**
         * @brief Draw the waypoint list, fading from red for the first to blue for the last.
         * @param spriteBatch The sprite batch to draw with.
         */
        void Draw(SpriteBatch& spriteBatch) const
        {
            int numberPoints = (int)getCountProperty() - 1;
            // This catches a special case where we have only one waypoint in the
            // list, in this case the waypoint won't draw correctly because we divide
            // 0 by 0 later on and get NaN for our result, fortunately for us this
            // doesn't cause the code to crash, but we end up getting a bad color
            // later on, so we catch this special case early and fix it
            if (numberPoints == 0)
            {
                numberPoints = 1;
            }

            float lerpAmt;
            float i = 0.0f;
            Color drawColor;

            spriteBatch.Begin();
            for (const Vector2& location : *this)
            {
                // This creates a gradient between 0 for the first waypoint on the
                // list and 1 for the last, 0 creates a color that's completely red
                // and 1 creates a color that's completely blue
                lerpAmt = i / (float)numberPoints;
                drawColor = Color(Vector4::Lerp(
                    Color::Red.ToVector4(), Color::Blue.ToVector4(), lerpAmt));

                spriteBatch.Draw(waypointTexture, location, std::nullopt, drawColor, 0.0f,
                                 waypointCenter, scale, SpriteEffects::None, 0.0f);

                i += 1.0f;
            }
            spriteBatch.End();
        }
    };
}
