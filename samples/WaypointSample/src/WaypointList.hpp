// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// WaypointList.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Collections/Generic/Queue.hpp"

namespace Waypoint
{
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using Microsoft::Xna::Framework::Graphics::SpriteBatch;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /**
     * @brief A queue of screen locations that draws itself.
     */
    class WaypointList : public System::Collections::Generic::Queue<Vector2>
    {
        // Draw data
        Texture2D waypointTexture;
        Vector2 waypointCenter;

    public:
        /**
         * @brief Loads the waypoint dot texture.
         * @param content The content manager to load from.
         */
        void LoadContent(ContentManager& content);

        /**
         * @brief Draws every waypoint, fading from red for the first to blue for the last.
         * @param spriteBatch The sprite batch to draw with.
         */
        void Draw(SpriteBatch& spriteBatch) const;
    };
}
