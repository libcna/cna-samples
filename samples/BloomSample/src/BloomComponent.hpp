// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// BloomComponent.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <optional>
#include <string>

#include "BloomSettings.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace BloomPostprocess
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    /**
     * @brief Component that applies a bloom postprocess over the scene the game draws.
     */
    class BloomComponent : public DrawableGameComponent
    {
    public:
        /**
         * @brief Constructs the bloom component for a game.
         * @param game The game that owns this component.
         */
        explicit BloomComponent(Game& game);

        /**
         * @brief Returns the fully qualified logical type name of this component.
         * @return "BloomPostprocess.BloomComponent".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Choose what display settings the bloom should use.
         * @return The settings currently in force.
         */
        [[nodiscard]] const BloomSettings& getSettingsProperty() const;

        /**
         * @brief Chooses what display settings the bloom should use.
         * @param value The settings to apply from the next frame on.
         */
        void setSettingsProperty(const BloomSettings& value);

        /**
         * @brief Optionally displays one of the intermediate buffers used by the bloom
         *        postprocess, so you can see exactly what is being drawn into each
         *        rendertarget.
         */
        enum class IntermediateBuffer
        {
            /** @brief The image after only the bright areas have been extracted. */
            PreBloom,
            /** @brief The extracted image after the horizontal blur pass. */
            BlurredHorizontally,
            /** @brief The extracted image after both blur passes. */
            BlurredBothWays,
            /** @brief The finished bloom, combined with the original scene. */
            FinalResult,
        };

        /**
         * @brief The intermediate buffer that is displayed.
         * @return The buffer the postprocess stops at.
         */
        [[nodiscard]] IntermediateBuffer getShowBufferProperty() const;

        /**
         * @brief Selects the intermediate buffer to display.
         * @param value The buffer the postprocess should stop at.
         */
        void setShowBufferProperty(IntermediateBuffer value);

        /**
         * @brief This should be called at the very start of the scene rendering.
         *
         * The bloom component uses it to redirect drawing into its custom rendertarget, so
         * it can capture the scene image in preparation for applying the bloom filter.
         */
        void BeginDraw();

        /**
         * @brief This is where it all happens.
         *
         * Grabs a scene that has already been rendered, and uses postprocess magic to add a
         * glowing bloom effect over the top of it.
         *
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

    protected:
        /** @brief Load your graphics content. */
        void LoadContent() override;

        /** @brief Unload your graphics content. */
        void UnloadContent() override;

    private:
        /**
         * Helper for drawing a texture into a rendertarget, using a custom shader to apply
         * postprocessing effects.
         */
        void DrawFullscreenQuad(Texture2D& texture, RenderTarget2D& renderTarget,
                                Effect* effect, IntermediateBuffer currentBuffer);

        /**
         * Helper for drawing a texture into the current rendertarget, using a custom shader
         * to apply postprocessing effects.
         */
        void DrawFullscreenQuad(Texture2D& texture, int width, int height,
                                Effect* effect, IntermediateBuffer currentBuffer);

        /**
         * Computes sample weightings and texture coordinate offsets for one pass of a
         * separable gaussian blur filter.
         */
        void SetBlurEffectParameters(float dx, float dy);

        /**
         * Evaluates a single point on the gaussian falloff curve. Used for setting up the
         * blur filter weightings.
         */
        [[nodiscard]] float ComputeGaussian(float n) const;

        std::unique_ptr<SpriteBatch> spriteBatch;

        // XNA's Effect and RenderTarget2D are reference types, so the original's fields are
        // null until LoadContent runs. The effects come out of the content manager as shared
        // references, exactly as the C# ones do.
        std::shared_ptr<Effect> bloomExtractEffect;
        std::shared_ptr<Effect> bloomCombineEffect;
        std::shared_ptr<Effect> gaussianBlurEffect;

        std::unique_ptr<RenderTarget2D> sceneRenderTarget;
        std::unique_ptr<RenderTarget2D> renderTarget1;
        std::unique_ptr<RenderTarget2D> renderTarget2;

        BloomSettings settings = BloomSettings::PresetSettings[0];

        IntermediateBuffer showBuffer = IntermediateBuffer::FinalResult;
    };
}
