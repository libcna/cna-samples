// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// LensFlareComponent.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "LensFlareComponent.hpp"

#include <algorithm>
#include <map>

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/ColorWriteChannels.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerStateCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"

namespace LensFlare
{
    namespace
    {
        // Custom blend state so the occlusion query polygons do not show up on screen.
        const BlendState& ColorWriteDisable()
        {
            static const BlendState state = [] {
                BlendState value;
                value.setColorWriteChannelsProperty(ColorWriteChannels::None);
                return value;
            }();
            return state;
        }

        // XNA's ContentManager returns the SAME Texture2D for a repeated asset name, and four of
        // the ten flares share "flare1". CNA's Load returns by value, so the cache is explicit.
        std::map<std::string, Texture2D>& TextureCache()
        {
            static std::map<std::string, Texture2D> cache;
            return cache;
        }
    }

    LensFlareComponent::LensFlareComponent(Game& game)
        : DrawableGameComponent(game)
    {
        flares = {
            {-0.5f, 0.7f, Color(50, 25, 50), "flare1", nullptr},
            {0.3f, 0.4f, Color(100, 255, 200), "flare1", nullptr},
            {1.2f, 1.0f, Color(100, 50, 50), "flare1", nullptr},
            {1.5f, 1.5f, Color(50, 100, 50), "flare1", nullptr},

            {-0.3f, 0.7f, Color(200, 50, 50), "flare2", nullptr},
            {0.6f, 0.9f, Color(50, 100, 50), "flare2", nullptr},
            {0.7f, 0.4f, Color(50, 200, 200), "flare2", nullptr},

            {-0.7f, 0.7f, Color(50, 100, 25), "flare3", nullptr},
            {0.0f, 0.6f, Color(25, 25, 25), "flare3", nullptr},
            {2.0f, 1.4f, Color(25, 50, 100), "flare3", nullptr},
        };
    }

    const std::string& LensFlareComponent::GetTypeName() const
    {
        static const std::string typeName{"LensFlare.LensFlareComponent"};
        return typeName;
    }

    void LensFlareComponent::LoadContent()
    {
        // Create a SpriteBatch for drawing the glow and flare sprites.
        spriteBatch.emplace(getGraphicsDeviceProperty());

        // Load the glow and flare textures.
        auto& cache = TextureCache();
        cache.clear();
        cache.emplace("glow", getGameProperty().getContentProperty().Load<Texture2D>("glow"));
        glowSprite = &cache.at("glow");

        for (Flare& flare : flares)
        {
            if (cache.find(flare.TextureName) == cache.end())
            {
                cache.emplace(flare.TextureName,
                              getGameProperty().getContentProperty()
                                  .Load<Texture2D>(flare.TextureName));
            }
            flare.Texture = &cache.at(flare.TextureName);
        }

        // Effect for drawing occlusion query polygons.
        basicEffect.emplace(getGraphicsDeviceProperty());

        basicEffect->setViewProperty(Matrix::getIdentityProperty());
        basicEffect->VertexColorEnabled = true;

        // Create vertex data for the occlusion query polygons.
        queryVertices[0].Position = Vector3(-querySize / 2, -querySize / 2, -1);
        queryVertices[1].Position = Vector3(querySize / 2, -querySize / 2, -1);
        queryVertices[2].Position = Vector3(-querySize / 2, querySize / 2, -1);
        queryVertices[3].Position = Vector3(querySize / 2, querySize / 2, -1);

        // Create the occlusion query object.
        occlusionQuery.emplace(getGraphicsDeviceProperty());
    }

    void LensFlareComponent::Draw(const GameTime& gameTime)
    {
        (void)gameTime;

        // Check whether the light is hidden behind the scenery.
        UpdateOcclusion();

        // Draw the flare effect.
        DrawGlow();
        DrawFlares();

        RestoreRenderStates();
    }

    void LensFlareComponent::UpdateOcclusion()
    {
        // The sun is infinitely distant, so it should not be affected by the
        // position of the camera. Floating point math doesn't support infinitely
        // distant vectors, but we can get the same result by making a copy of our
        // view matrix, then resetting the view translation to zero. Pretending the
        // camera has not moved position gives the same result as if the camera
        // was moving, but the light was infinitely far away. If our flares came
        // from a local object rather than the sun, we would use the original view
        // matrix here.
        Matrix infiniteView = View;

        infiniteView.setTranslationProperty(Vector3::Zero);

        // Project the light position into 2D screen space.
        GraphicsDevice& device = getGraphicsDeviceProperty();
        const Viewport viewport = device.getViewportProperty();

        const Vector3 projectedPosition =
            viewport.Project(-LightDirection, Projection, infiniteView, Matrix::getIdentityProperty());

        // Don't draw any flares if the light is behind the camera.
        if ((projectedPosition.Z < 0) || (projectedPosition.Z > 1))
        {
            lightBehindCamera = true;
            return;
        }

        lightPosition = Vector2(projectedPosition.X, projectedPosition.Y);
        lightBehindCamera = false;

        if (occlusionQueryActive)
        {
            // If the previous query has not yet completed, wait until it does.
            if (!occlusionQuery->getIsCompleteProperty())
                return;

            // Use the occlusion query pixel count to work
            // out what percentage of the sun is visible.
            constexpr float queryArea = querySize * querySize;

            occlusionAlpha =
                std::min((float)occlusionQuery->getPixelCountProperty() / queryArea, 1.0f);
        }

        // Set renderstates for drawing the occlusion query geometry. We want depth
        // tests enabled, but depth writes disabled, and we disable color writes
        // to prevent this query polygon actually showing up on the screen.
        device.setBlendStateProperty(ColorWriteDisable());
        device.setDepthStencilStateProperty(DepthStencilState::DepthRead);

        // Set up our BasicEffect to center on the current 2D light position.
        basicEffect->setWorldProperty(
            Matrix::CreateTranslation(lightPosition.X, lightPosition.Y, 0));

        basicEffect->setProjectionProperty(
            Matrix::CreateOrthographicOffCenter(0,
                                                (float)viewport.getWidthProperty(),
                                                (float)viewport.getHeightProperty(),
                                                0, 0, 1));

        basicEffect->getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();

        // Issue the occlusion query.
        occlusionQuery->Begin();

        device.DrawUserPrimitives(PrimitiveType::TriangleStrip, queryVertices.data(), 0, 2,
                                  VertexPositionColor::getVertexDeclarationStatic());

        occlusionQuery->End();

        occlusionQueryActive = true;
    }

    void LensFlareComponent::DrawGlow()
    {
        if (lightBehindCamera || occlusionAlpha <= 0)
            return;

        const Color color = Color::White * occlusionAlpha;
        const Vector2 origin = Vector2((float)glowSprite->getWidthProperty(),
                                       (float)glowSprite->getHeightProperty()) / 2;
        const float scale = glowSize * 2 / (float)glowSprite->getWidthProperty();

        spriteBatch->Begin();

        spriteBatch->Draw(*glowSprite, lightPosition, std::nullopt, color, 0,
                          origin, scale, SpriteEffects::None, 0);

        spriteBatch->End();
    }

    void LensFlareComponent::DrawFlares()
    {
        if (lightBehindCamera || occlusionAlpha <= 0)
            return;

        const Viewport viewport = getGraphicsDeviceProperty().getViewportProperty();

        // Lensflare sprites are positioned at intervals along a line that
        // runs from the 2D light position toward the center of the screen.
        const Vector2 screenCenter = Vector2((float)viewport.getWidthProperty(),
                                             (float)viewport.getHeightProperty()) / 2;

        const Vector2 flareVector = screenCenter - lightPosition;

        // Draw the flare sprites using additive blending.
        spriteBatch->Begin(SpriteSortMode::Deferred, BlendState::Additive);

        for (const Flare& flare : flares)
        {
            // Compute the position of this flare sprite.
            const Vector2 flarePosition = lightPosition + flareVector * flare.Position;

            // Set the flare alpha based on the previous occlusion query result.
            Vector4 flareColor = flare.Color.ToVector4();

            flareColor.W *= occlusionAlpha;

            // Center the sprite texture.
            const Vector2 flareOrigin = Vector2((float)flare.Texture->getWidthProperty(),
                                                (float)flare.Texture->getHeightProperty()) / 2;

            // Draw the flare.
            spriteBatch->Draw(*flare.Texture, flarePosition, std::nullopt,
                              Color(flareColor), 1, flareOrigin,
                              flare.Scale, SpriteEffects::None, 0);
        }

        spriteBatch->End();
    }

    void LensFlareComponent::RestoreRenderStates()
    {
        GraphicsDevice& device = getGraphicsDeviceProperty();

        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.getSamplerStatesProperty()[0] = SamplerState::LinearWrap;
    }
}
