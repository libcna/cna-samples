// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// DistortionComponent.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace DistortionSample
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;
    using SharpRuntime::String;

    class Distorter;

    /**
     * @brief Renders a distortion map over the scene and warps the scene with it.
     */
    class DistortionComponent : public DrawableGameComponent
    {
    public:
        /** @brief The distortion techniques `Distorters.fx` implements, by name. */
        enum class DistortionTechnique
        {
            /** @brief Warps by a displacement map painted onto the distorter. */
            DisplacementMapped,
            /** @brief A rising, time-varying heat shimmer. */
            HeatHaze,
            /** @brief Pulls the image in towards the distorter's centre. */
            PullIn,
            /** @brief Displaces nothing, so the distorter is invisible. */
            ZeroDisplacement,
        };

        /**
         * @brief The name a technique is shown under in the overlay.
         * @param technique The technique to name.
         * @return Its friendly name, such as "Heat-Haze".
         */
        [[nodiscard]] static const String& GetDistortionTechniqueFriendlyName(
            DistortionTechnique technique);

        /**
         * @brief The name of the `Distorters.fx` technique that implements it.
         *
         * C# gets this from `Enum.ToString()`, which C++ has no equivalent of.
         *
         * @param technique The technique to name.
         * @return Its effect-technique name, such as "HeatHaze".
         */
        CNAEXT [[nodiscard]] static const String& ToString(DistortionTechnique technique);

        /**
         * @brief Constructs the distortion component for a game.
         * @param game The game that owns this component.
         */
        explicit DistortionComponent(Game& game);

        /**
         * @brief Returns the fully qualified logical type name of this component.
         * @return "DistortionSample.DistortionComponent".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief This should be called at the very start of scene rendering.
         *
         * The distortion component uses it to redirect drawing into its custom
         * rendertarget, so it can capture the scene image ready to apply the distortion
         * postprocess.
         */
        void BeginDraw();

        /**
         * @brief Grabs a scene that has already been rendered, and adds a distortion
         *        effect over the top of it.
         *
         * @param gameTime Provides a snapshot of timing values.
         */
        void Draw(const GameTime& gameTime) override;

        /** @brief The view matrix the distorter is drawn with. */
        Matrix View;

        /** @brief The projection matrix the distorter is drawn with. */
        Matrix Projection;

        /** @brief The distorter to draw, or null to draw none. */
        Distorter* Distorter_ = nullptr;

        /** @brief True to show the raw distortion map instead of the distorted scene. */
        bool ShowDistortionMap = false;

    protected:
        /** @brief Load your graphics content. */
        void LoadContent() override;

        /** @brief Unload your graphics content. */
        void UnloadContent() override;

    private:
        /**
         * Helper for drawing a texture into the current rendertarget, using a custom
         * shader to apply postprocessing effects.
         */
        void DrawFullscreenQuad(Texture2D& texture, int width, int height, Effect* effect);

        /**
         * Computes sample weightings and texture coordinate offsets for one pass of a
         * separable gaussian blur filter.
         *
         * This function was originally provided in the BloomComponent class in the Bloom
         * Postprocess sample.
         */
        void SetBlurEffectParameters(float dx, float dy);

        /**
         * Evaluates a single point on the gaussian falloff curve. Used for setting up the
         * blur filter weightings.
         *
         * This function was originally provided in the BloomComponent class in the Bloom
         * Postprocess sample.
         */
        [[nodiscard]] static float ComputeGaussian(float n);

        static constexpr float blurAmount = 2.0f;

        std::unique_ptr<SpriteBatch> spriteBatch;
        std::unique_ptr<RenderTarget2D> sceneMap;
        std::unique_ptr<RenderTarget2D> distortionMap;

        // XNA's Effect is a reference type, so the original's field is null until
        // LoadContent runs; the techniques are borrowed from it.
        std::shared_ptr<Effect> distortEffect;
        EffectTechnique* distortTechnique = nullptr;
        EffectTechnique* distortBlurTechnique = nullptr;
    };
}
