// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// LensFlareComponent.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/OcclusionQuery.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"

namespace LensFlare
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Reusable component for drawing a lensflare effect over the top of a 3D scene.
     */
    class LensFlareComponent : public Microsoft::Xna::Framework::DrawableGameComponent
    {
    public:
        /**
         * @brief Constructs a new lensflare component.
         * @param game The game this component belongs to.
         */
        explicit LensFlareComponent(Game& game);

        /**
         * @brief Returns the fully qualified logical type name of this component.
         * @return "LensFlare.LensFlareComponent".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Draws the lensflare component.
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

        /**
         * @brief Mesures how much of the sun is visible, by drawing a small rectangle, centered on
         *        the sun, but with the depth set to as far away as possible, and using an occlusion
         *        query to measure how many of these very-far-away pixels are not hidden behind the
         *        terrain.
         *
         * The occlusion results are read back one frame late, because waiting for the graphics card
         * to finish processing the commands it has buffered would slow the game down too much.
         */
        void UpdateOcclusion();

        /** @brief Draws a large circular glow sprite, centered on the sun. */
        void DrawGlow();

        /**
         * @brief Draws the lensflare sprites, computing the position of each one based on the
         *        current angle of the sun.
         */
        void DrawFlares();

        // These are set by the main game to tell us the position of the camera and sun.

        /** @brief The camera's view matrix, set by the main game. */
        Matrix View;

        /** @brief The camera's projection matrix, set by the main game. */
        Matrix Projection;

        /** @brief Direction the sunlight is travelling in. */
        Vector3 LightDirection = Vector3::Normalize(Vector3(-1, -0.1f, 0.3f));

    protected:
        /** @brief Loads the content used by the lensflare component. */
        void LoadContent() override;

    private:
        /**
         * Sets renderstates back to their default values after we finish drawing the lensflare,
         * to avoid messing up the 3D terrain rendering.
         */
        void RestoreRenderStates();

        // How big is the circular glow effect?
        static constexpr float glowSize = 400;

        // How big a rectangle should we examine when issuing our occlusion queries?
        // Increasing this makes the flares fade out more gradually when the sun goes
        // behind scenery, while smaller query areas cause sudden on/off transitions.
        static constexpr float querySize = 100;

        // The lensflare effect is made up from several individual flare graphics,
        // which move across the screen depending on the position of the sun. This
        // helper class keeps track of the position, size, and color for each flare.
        struct Flare
        {
            float Position;
            float Scale;
            Microsoft::Xna::Framework::Color Color;
            std::string TextureName;
            const Texture2D* Texture = nullptr;
        };

        // Computed by UpdateOcclusion, which projects LightDirection into screenspace.
        Vector2 lightPosition;
        bool lightBehindCamera = false;

        // Graphics objects.
        const Texture2D* glowSprite = nullptr;
        std::optional<SpriteBatch> spriteBatch;
        std::optional<BasicEffect> basicEffect;
        std::array<VertexPositionColor, 4> queryVertices;

        // An occlusion query is used to detect when the sun is hidden behind scenery.
        std::optional<OcclusionQuery> occlusionQuery;
        bool occlusionQueryActive = false;
        float occlusionAlpha = 0;

        // Array describes the position, size, color, and texture for each individual
        // flare graphic. The position value lies on a line between the sun and the
        // center of the screen. Zero places a flare directly over the top of the sun,
        // one is exactly in the middle of the screen, fractional positions lie in
        // between these two points, while negative values or positions greater than
        // one will move the flares outward toward the edge of the screen. Changing
        // the number of flares, or tweaking their positions and colors, can produce
        // a wide range of different lensflare effects without altering any other code.
        std::vector<Flare> flares;
    };
}
