// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// DistortionComponent.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "DistortionComponent.hpp"

#include "Distorter.hpp"

#include <cmath>
#include <numbers>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameterCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechniqueCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/PresentationParameters.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"

namespace DistortionSample
{
    namespace
    {
        const String kFriendlyNames[] = {
            "Displacement-Mapped",
            "Heat-Haze",
            "Pull-In",
            "Zero Displacement",
        };
        const String kTechniqueNames[] = {
            "DisplacementMapped",
            "HeatHaze",
            "PullIn",
            "ZeroDisplacement",
        };
    }

    const String& DistortionComponent::GetDistortionTechniqueFriendlyName(
        const DistortionTechnique technique)
    {
        return kFriendlyNames[(std::size_t)technique];
    }

    const String& DistortionComponent::ToString(const DistortionTechnique technique)
    {
        return kTechniqueNames[(std::size_t)technique];
    }

    DistortionComponent::DistortionComponent(Game& game)
        : DrawableGameComponent(game)
    {
    }

    const std::string& DistortionComponent::GetTypeName() const
    {
        static const std::string name = "DistortionSample.DistortionComponent";
        return name;
    }

    void DistortionComponent::LoadContent()
    {
        spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
        distortEffect =
            getGameProperty().getContentProperty().Load<std::shared_ptr<Effect>>("Distort");
        distortTechnique = distortEffect->getTechniquesProperty()["Distort"];
        distortBlurTechnique = distortEffect->getTechniquesProperty()["DistortBlur"];

        // update the projection matrix
        Projection = Matrix::CreatePerspectiveFieldOfView(
            1.0f, getGraphicsDeviceProperty().getViewportProperty().getAspectRatioProperty(),
            1.0f, 10000.0f);

        // look up the resolution and format of our main backbuffer
        const PresentationParameters& pp =
            getGraphicsDeviceProperty().getPresentationParametersProperty();
        const int width = pp.getBackBufferWidthProperty();
        const int height = pp.getBackBufferHeightProperty();
        const SurfaceFormat format = pp.getBackBufferFormatProperty();
        const DepthFormat depthFormat = pp.getDepthStencilFormatProperty();

        // create textures for reading back the backbuffer contents
        sceneMap = std::make_unique<RenderTarget2D>(
            getGraphicsDeviceProperty(), width, height, false, format, depthFormat);
        distortionMap = std::make_unique<RenderTarget2D>(
            getGraphicsDeviceProperty(), width, height, false, format, depthFormat);

        // set the blur parameters for the current viewport
        SetBlurEffectParameters(1.0f / (float)width, 1.0f / (float)height);
    }

    void DistortionComponent::UnloadContent()
    {
        spriteBatch = nullptr;
        distortEffect = nullptr;
        distortTechnique = nullptr;
        distortBlurTechnique = nullptr;

        if (sceneMap != nullptr)
        {
            sceneMap->Dispose();
            sceneMap = nullptr;
        }
        if (distortionMap != nullptr)
        {
            distortionMap->Dispose();
            distortionMap = nullptr;
        }
    }

    void DistortionComponent::BeginDraw()
    {
        if (getVisibleProperty())
        {
            getGraphicsDeviceProperty().SetRenderTarget(sceneMap.get());
        }
    }

    void DistortionComponent::Draw(const GameTime& gameTime)
    {
        // now draw the distortion map
        getGraphicsDeviceProperty().SetRenderTarget(
            ShowDistortionMap ? nullptr : distortionMap.get());
        getGraphicsDeviceProperty().Clear(Color::Transparent);

        // draw the distorter
        if (Distorter_ != nullptr)
        {
            const Matrix worldView = Distorter_->World * View;
            std::vector<Matrix> transforms(
                (std::size_t)Distorter_->Model_->getBonesProperty().getCountProperty());
            Distorter_->Model_->CopyAbsoluteBoneTransformsTo(transforms);

            // make sure the depth buffering is on, so only parts of the scene
            // behind the distortion effect are affected
            getGraphicsDeviceProperty().setDepthStencilStateProperty(
                DepthStencilState::Default);

            for (ModelMesh* mesh : Distorter_->Model_->getMeshesProperty())
            {
                const Matrix meshWorldView =
                    transforms[(std::size_t)mesh->getParentBoneProperty()->getIndexProperty()] *
                    worldView;
                for (Effect* effect : mesh->getEffectsProperty())
                {
                    effect->setCurrentTechniqueProperty(
                        effect->getTechniquesProperty()[ToString(Distorter_->Technique)]);
                    effect->getParametersProperty()["WorldView"]->SetValue(meshWorldView);
                    effect->getParametersProperty()["WorldViewProjection"]->SetValue(
                        meshWorldView * Projection);
                    effect->getParametersProperty()["DistortionScale"]->SetValue(
                        Distorter_->DistortionScale);
                    effect->getParametersProperty()["Time"]->SetValue(
                        (float)gameTime.getTotalGameTimeProperty().getTotalSecondsProperty());
                }
                mesh->Draw();
            }
        }

        // if we want to show the distortion map, then the backbuffer is done.
        // if we want to render the scene distorted, then we need to resolve the
        // backbuffer as the distortion map and use it to distort the scene
        if (!ShowDistortionMap)
        {
            getGraphicsDeviceProperty().SetRenderTarget(nullptr);

            // draw the scene image again, distorting it with the distortion map
            getGraphicsDeviceProperty().getTexturesProperty()(1, distortionMap.get());
            getGraphicsDeviceProperty().getSamplerStatesProperty()[1] =
                SamplerState::PointClamp;
            const Viewport viewport = getGraphicsDeviceProperty().getViewportProperty();
            distortEffect->setCurrentTechniqueProperty(
                Distorter_->DistortionBlur ? distortBlurTechnique : distortTechnique);
            DrawFullscreenQuad(*sceneMap, viewport.getWidthProperty(),
                               viewport.getHeightProperty(), distortEffect.get());
        }
    }

    void DistortionComponent::DrawFullscreenQuad(
        Texture2D& texture, const int width, const int height, Effect* const effect)
    {
        spriteBatch->Begin(SpriteSortMode::Deferred, BlendState::Opaque,
                           nullptr, nullptr, nullptr, effect);
        spriteBatch->Draw(texture, Rectangle(0, 0, width, height), Color::White);
        spriteBatch->End();
    }

    void DistortionComponent::SetBlurEffectParameters(const float dx, const float dy)
    {
        // Look up the sample weight and offset effect parameters.
        EffectParameter* const weightsParameter =
            distortEffect->getParametersProperty()["SampleWeights"];
        EffectParameter* const offsetsParameter =
            distortEffect->getParametersProperty()["SampleOffsets"];

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

    float DistortionComponent::ComputeGaussian(const float n)
    {
        // Math.PI and Math.Sqrt/Math.Exp are double in C#, while the exponent itself is
        // computed from float values; the casts keep each operation on the same width.
        return (float)((1.0 / std::sqrt(2 * std::numbers::pi_v<double> * (double)blurAmount)) *
                       std::exp((double)(-(n * n) / (2 * blurAmount * blurAmount))));
    }
}
