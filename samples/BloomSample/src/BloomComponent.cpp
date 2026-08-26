// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// BloomComponent.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "BloomComponent.hpp"

#include <cmath>
#include <numbers>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/PresentationParameters.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTargetUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"

namespace BloomPostprocess
{
    BloomComponent::BloomComponent(Game& game)
        : DrawableGameComponent(game)
    {
        // The original throws ArgumentNullException for a null game. A C++ reference cannot
        // be null, so the check has nothing left to test.
    }

    const std::string& BloomComponent::GetTypeName() const
    {
        static const std::string name = "BloomPostprocess.BloomComponent";
        return name;
    }

    const BloomSettings& BloomComponent::getSettingsProperty() const { return settings; }

    void BloomComponent::setSettingsProperty(const BloomSettings& value) { settings = value; }

    BloomComponent::IntermediateBuffer BloomComponent::getShowBufferProperty() const
    {
        return showBuffer;
    }

    void BloomComponent::setShowBufferProperty(IntermediateBuffer value) { showBuffer = value; }

    void BloomComponent::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());

        bloomExtractEffect =
            getGameProperty().getContentProperty().Load<std::shared_ptr<Effect>>("BloomExtract");
        bloomCombineEffect =
            getGameProperty().getContentProperty().Load<std::shared_ptr<Effect>>("BloomCombine");
        gaussianBlurEffect =
            getGameProperty().getContentProperty().Load<std::shared_ptr<Effect>>("GaussianBlur");

        // Look up the resolution and format of our main backbuffer.
        const PresentationParameters& pp =
            getGraphicsDeviceProperty().getPresentationParametersProperty();

        int width = pp.getBackBufferWidthProperty();
        int height = pp.getBackBufferHeightProperty();

        const SurfaceFormat format = pp.getBackBufferFormatProperty();

        // Create a texture for rendering the main scene, prior to applying bloom.
        sceneRenderTarget = std::make_unique<RenderTarget2D>(
            getGraphicsDeviceProperty(), width, height, false,
            format, pp.getDepthStencilFormatProperty(), pp.getMultiSampleCountProperty(),
            RenderTargetUsage::DiscardContents);

        // Create two rendertargets for the bloom processing. These are half the
        // size of the backbuffer, in order to minimize fillrate costs. Reducing
        // the resolution in this way doesn't hurt quality, because we are going
        // to be blurring the bloom images in any case.
        width /= 2;
        height /= 2;

        renderTarget1 = std::make_unique<RenderTarget2D>(
            getGraphicsDeviceProperty(), width, height, false, format, DepthFormat::None);
        renderTarget2 = std::make_unique<RenderTarget2D>(
            getGraphicsDeviceProperty(), width, height, false, format, DepthFormat::None);
    }

    void BloomComponent::UnloadContent()
    {
        sceneRenderTarget->Dispose();
        renderTarget1->Dispose();
        renderTarget2->Dispose();
    }

    void BloomComponent::BeginDraw()
    {
        if (getVisibleProperty())
        {
            getGraphicsDeviceProperty().SetRenderTarget(sceneRenderTarget.get());
        }
    }

    void BloomComponent::Draw(const GameTime& gameTime)
    {
        (void)gameTime;

        getGraphicsDeviceProperty().getSamplerStatesProperty()[1] = SamplerState::LinearClamp;

        // Pass 1: draw the scene into rendertarget 1, using a
        // shader that extracts only the brightest parts of the image.
        bloomExtractEffect->getParametersProperty()["BloomThreshold"]->SetValue(
            getSettingsProperty().BloomThreshold);

        DrawFullscreenQuad(*sceneRenderTarget, *renderTarget1,
                           bloomExtractEffect.get(),
                           IntermediateBuffer::PreBloom);

        // Pass 2: draw from rendertarget 1 into rendertarget 2,
        // using a shader to apply a horizontal gaussian blur filter.
        SetBlurEffectParameters(1.0f / (float)renderTarget1->getWidthProperty(), 0);

        DrawFullscreenQuad(*renderTarget1, *renderTarget2,
                           gaussianBlurEffect.get(),
                           IntermediateBuffer::BlurredHorizontally);

        // Pass 3: draw from rendertarget 2 back into rendertarget 1,
        // using a shader to apply a vertical gaussian blur filter.
        SetBlurEffectParameters(0, 1.0f / (float)renderTarget1->getHeightProperty());

        DrawFullscreenQuad(*renderTarget2, *renderTarget1,
                           gaussianBlurEffect.get(),
                           IntermediateBuffer::BlurredBothWays);

        // Pass 4: draw both rendertarget 1 and the original scene
        // image back into the main backbuffer, using a shader that
        // combines them to produce the final bloomed result.
        getGraphicsDeviceProperty().SetRenderTarget(nullptr);

        EffectParameterCollection& parameters = bloomCombineEffect->getParametersProperty();

        parameters["BloomIntensity"]->SetValue(getSettingsProperty().BloomIntensity);
        parameters["BaseIntensity"]->SetValue(getSettingsProperty().BaseIntensity);
        parameters["BloomSaturation"]->SetValue(getSettingsProperty().BloomSaturation);
        parameters["BaseSaturation"]->SetValue(getSettingsProperty().BaseSaturation);

        getGraphicsDeviceProperty().getTexturesProperty()(1, sceneRenderTarget.get());

        const Viewport viewport = getGraphicsDeviceProperty().getViewportProperty();

        DrawFullscreenQuad(*renderTarget1,
                           viewport.getWidthProperty(), viewport.getHeightProperty(),
                           bloomCombineEffect.get(),
                           IntermediateBuffer::FinalResult);
    }

    void BloomComponent::DrawFullscreenQuad(Texture2D& texture, RenderTarget2D& renderTarget,
                                            Effect* effect, IntermediateBuffer currentBuffer)
    {
        getGraphicsDeviceProperty().SetRenderTarget(&renderTarget);

        DrawFullscreenQuad(texture,
                           renderTarget.getWidthProperty(), renderTarget.getHeightProperty(),
                           effect, currentBuffer);
    }

    void BloomComponent::DrawFullscreenQuad(Texture2D& texture, int width, int height,
                                            Effect* effect, IntermediateBuffer currentBuffer)
    {
        // If the user has selected one of the show intermediate buffer options,
        // we still draw the quad to make sure the image will end up on the screen,
        // but might need to skip applying the custom pixel shader.
        if (showBuffer < currentBuffer)
        {
            effect = nullptr;
        }

        spriteBatch->Begin(SpriteSortMode::Deferred, BlendState::Opaque,
                           nullptr, nullptr, nullptr, effect);
        spriteBatch->Draw(texture, Rectangle(0, 0, width, height), Color::White);
        spriteBatch->End();
    }

    void BloomComponent::SetBlurEffectParameters(float dx, float dy)
    {
        // Look up the sample weight and offset effect parameters.
        EffectParameter* weightsParameter =
            gaussianBlurEffect->getParametersProperty()["SampleWeights"];
        EffectParameter* offsetsParameter =
            gaussianBlurEffect->getParametersProperty()["SampleOffsets"];

        // Look up how many samples our gaussian blur effect supports.
        const int sampleCount = weightsParameter->getElementsProperty().getCountProperty();

        // Create temporary arrays for computing our filter settings.
        std::vector<float> sampleWeights((std::size_t)sampleCount);
        std::vector<Vector2> sampleOffsets((std::size_t)sampleCount);

        // The first sample always has a zero offset.
        sampleWeights[0] = ComputeGaussian(0);
        sampleOffsets[0] = Vector2(0);

        // Maintain a sum of all the weighting values.
        float totalWeights = sampleWeights[0];

        // Add pairs of additional sample taps, positioned
        // along a line in both directions from the center.
        for (int i = 0; i < sampleCount / 2; i++)
        {
            // Store weights for the positive and negative taps.
            const float weight = ComputeGaussian((float)(i + 1));

            sampleWeights[(std::size_t)(i * 2 + 1)] = weight;
            sampleWeights[(std::size_t)(i * 2 + 2)] = weight;

            totalWeights += weight * 2;

            // To get the maximum amount of blurring from a limited number of
            // pixel shader samples, we take advantage of the bilinear filtering
            // hardware inside the texture fetch unit. If we position our texture
            // coordinates exactly halfway between two texels, the filtering unit
            // will average them for us, giving two samples for the price of one.
            // This allows us to step in units of two texels per sample, rather
            // than just one at a time. The 1.5 offset kicks things off by
            // positioning us nicely in between two texels.
            const float sampleOffset = (float)i * 2 + 1.5f;

            const Vector2 delta = Vector2(dx, dy) * sampleOffset;

            // Store texture coordinate offsets for the positive and negative taps.
            sampleOffsets[(std::size_t)(i * 2 + 1)] = delta;
            sampleOffsets[(std::size_t)(i * 2 + 2)] = -delta;
        }

        // Normalize the list of sample weightings, so they will always sum to one.
        for (std::size_t i = 0; i < sampleWeights.size(); i++)
        {
            sampleWeights[i] /= totalWeights;
        }

        // Tell the effect about our new filter settings.
        weightsParameter->SetValue(sampleWeights);
        offsetsParameter->SetValue(sampleOffsets);
    }

    float BloomComponent::ComputeGaussian(float n) const
    {
        const float theta = getSettingsProperty().BlurAmount;

        // Math.PI and Math.Sqrt/Math.Exp are double in C#, while the exponent itself is
        // computed from float fields; the casts keep each operation on the same width.
        return (float)((1.0 / std::sqrt(2 * std::numbers::pi_v<double> * (double)theta)) *
                       std::exp((double)(-(n * n) / (2 * theta * theta))));
    }
}
