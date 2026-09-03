// SPDX-License-Identifier: MS-PL

#include "Rendering/ShadowMapRenderer.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Blend.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendFunction.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/ClearOptions.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PackedVector/Rgba64.hpp"
#include "Microsoft/Xna/Framework/Graphics/PresentationParameters.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTargetUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Rendering/CarRenderer.hpp"
#include "Rendering/StaticTrackScene.hpp"

namespace RacingGame::Rendering
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using namespace Microsoft::Xna::Framework::Graphics;

    ShadowMapRenderer::ShadowMapRenderer(
        GraphicsDevice& setDevice,
        Microsoft::Xna::Framework::Content::ContentManager& content,
        const bool highDetail)
        : device(setDevice),
          shadowEffect(content.Load<std::shared_ptr<Effect>>(
              "Shaders/ShadowMap")),
          blurEffect(content.Load<std::shared_ptr<Effect>>(
              "Shaders/PostScreenShadowBlur")),
          fadeTexture(content.Load<Texture2D>(
              "Textures/ShadowDistanceFadeoutMap")),
          shadowMap(device, Shaders::RenderToTexture::SizeType::ShadowMap,
                    highDetail),
          sceneMap(device, Shaders::RenderToTexture::SizeType::HalfScreen,
                   highDetail),
          blurMap(device, Shaders::RenderToTexture::SizeType::HalfScreen,
                  highDetail),
          shadowMapSize(shadowMap.getWidthProperty()),
          fullscreenQuad(
              device, VertexPositionTexture::getVertexDeclarationStatic(),
              4, BufferUsage::WriteOnly)
    {
        if (!shadowEffect || !blurEffect)
            throw std::runtime_error(
                "Authentic Racing shadow effects failed to load");
        EffectTechnique* generate =
            shadowEffect->getTechniquesProperty()["GenerateShadowMap20"];
        EffectTechnique* use =
            shadowEffect->getTechniquesProperty()["UseShadowMap20"];
        EffectTechnique* blur =
            blurEffect->getTechniquesProperty()["ScreenAdvancedBlur20"];
        if (!generate || !use || !blur ||
            blur->getPassesProperty().getCountProperty() != 2)
            throw std::runtime_error(
                "Authentic Racing shadow techniques are incomplete");

        const std::array<VertexPositionTexture, 4> vertices = {{
            {Vector3(-1.0f, -1.0f, 0.5f), Vector2(0.0f, 1.0f)},
            {Vector3(-1.0f, 1.0f, 0.5f), Vector2(0.0f, 0.0f)},
            {Vector3(1.0f, -1.0f, 0.5f), Vector2(1.0f, 1.0f)},
            {Vector3(1.0f, 1.0f, 0.5f), Vector2(1.0f, 0.0f)},
        }};
        fullscreenQuad.SetData(vertices.data(),
                               static_cast<int>(vertices.size()));

        zeroSourceBlend.setAlphaBlendFunctionProperty(BlendFunction::Add);
        zeroSourceBlend.setColorBlendFunctionProperty(BlendFunction::Add);
        zeroSourceBlend.setAlphaSourceBlendProperty(Blend::Zero);
        zeroSourceBlend.setColorSourceBlendProperty(Blend::Zero);
        zeroSourceBlend.setAlphaDestinationBlendProperty(Blend::SourceAlpha);
        zeroSourceBlend.setColorDestinationBlendProperty(Blend::SourceColor);

        textureScaleBias = Matrix(
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, -0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.5f + 0.5f / shadowMapSize,
            0.5f + 0.5f / shadowMapSize, 0.0f, 1.0f);
    }

    void ShadowMapRenderer::SetSharedShadowParameters()
    {
        auto& parameters = shadowEffect->getParametersProperty();
        if (EffectParameter* parameter = parameters["depthBias"])
            parameter->SetValue(DepthBias);
        if (EffectParameter* parameter = parameters["shadowMapDepthBias"])
            parameter->SetValue(DepthBias);
        if (EffectParameter* parameter = parameters["shadowMapTexelSize"])
            parameter->SetValue(Vector2(
                1.0f / shadowMapSize, 1.0f / shadowMapSize));
        if (EffectParameter* parameter = parameters["nearPlane"])
            parameter->SetValue(ShadowNearPlane);
        if (EffectParameter* parameter = parameters["farPlane"])
            parameter->SetValue(ShadowFarPlane);
        if (EffectParameter* parameter =
                parameters["shadowDistanceFadeoutTexture"])
            parameter->SetValue(&fadeTexture);
    }

    void ShadowMapRenderer::Prepare(
        StaticTrackScene& scene, CarRenderer& car, const Matrix carMatrix,
        const Vector3 carPosition, const Vector3 carDirection,
        const Matrix& view, const Matrix& projection,
        const float totalTimeSeconds)
    {
        const float virtualFieldOfView = std::atan2(
            VirtualVisibleRange, ShadowDistance);
        const Matrix lightProjection = Matrix::CreatePerspective(
            virtualFieldOfView, 1.0f, ShadowNearPlane, ShadowFarPlane);
        const Vector3 lightLookPosition =
            carPosition + carDirection * VirtualVisibleRange / 6.0f;
        const Vector3 lightDirection = Vector3::Normalize(
            Vector3(8500.0f, -7250.0f, 15000.0f));
        const Matrix lightView = Matrix::CreateLookAt(
            lightLookPosition + lightDirection * VirtualVisibleRange,
            lightLookPosition, Vector3::UnitZ);
        lightViewProjection = lightView * lightProjection;
        shadowLightPosition =
            Matrix::Invert(lightView).getTranslationProperty();

        SetSharedShadowParameters();
        shadowEffect->setCurrentTechniqueProperty(
            shadowEffect->getTechniquesProperty()["GenerateShadowMap20"]);
        shadowMap.SetRenderTarget();
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setBlendStateProperty(BlendState::Opaque);
        device.Clear(Color::White);
        lastCasterSubmissions = scene.GenerateShadows(
            *shadowEffect, lightViewProjection, shadowLightPosition,
            ShadowDistance, totalTimeSeconds);
        lastCasterSubmissions += car.GenerateShadow(
            *shadowEffect, carMatrix, lightViewProjection);
        shadowMap.Resolve();

        shadowEffect->setCurrentTechniqueProperty(
            shadowEffect->getTechniquesProperty()["UseShadowMap20"]);
        SetSharedShadowParameters();
        if (EffectParameter* parameter =
                shadowEffect->getParametersProperty()["shadowMap"])
            parameter->SetValue(&shadowMap.getXnaTextureProperty());
        sceneMap.SetRenderTarget();
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setBlendStateProperty(BlendState::Opaque);
        device.Clear(Color::White);
        const Matrix viewProjection = view * projection;
        lastReceiverSubmissions = scene.UseShadows(
            *shadowEffect, viewProjection, lightViewProjection,
            textureScaleBias, shadowLightPosition, ShadowDistance,
            totalTimeSeconds);
        lastReceiverSubmissions += car.UseShadow(
            *shadowEffect, carMatrix, viewProjection, lightViewProjection,
            textureScaleBias);
        sceneMap.Resolve();

        FinishShadowPreparation();
    }

    void ShadowMapRenderer::PrepareCarSelection(
        CarRenderer& car, const std::array<Matrix, 3>& renderMatrices,
        const Matrix& view, const Matrix& projection)
    {
        const float virtualFieldOfView = std::atan2(
            VirtualVisibleRange, ShadowDistance);
        const Matrix lightProjection = Matrix::CreatePerspective(
            virtualFieldOfView, 1.0f, ShadowNearPlane, ShadowFarPlane);
        const Vector3 lightDirection = Vector3::Normalize(
            Vector3(-8500.0f, 7250.0f, 15000.0f));
        const Matrix lightView = Matrix::CreateLookAt(
            lightDirection * VirtualVisibleRange, Vector3::Zero,
            Vector3::UnitZ);
        lightViewProjection = lightView * lightProjection;
        shadowLightPosition =
            Matrix::Invert(lightView).getTranslationProperty();

        SetSharedShadowParameters();
        shadowEffect->setCurrentTechniqueProperty(
            shadowEffect->getTechniquesProperty()["GenerateShadowMap20"]);
        shadowMap.SetRenderTarget();
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setBlendStateProperty(BlendState::Opaque);
        device.Clear(Color::White);
        lastCasterSubmissions = 0;
        for (const Matrix& matrix : renderMatrices)
            lastCasterSubmissions += car.GenerateShadow(
                *shadowEffect, matrix, lightViewProjection);
        shadowMap.Resolve();

        shadowEffect->setCurrentTechniqueProperty(
            shadowEffect->getTechniquesProperty()["UseShadowMap20"]);
        SetSharedShadowParameters();
        if (EffectParameter* parameter =
                shadowEffect->getParametersProperty()["shadowMap"])
            parameter->SetValue(&shadowMap.getXnaTextureProperty());
        sceneMap.SetRenderTarget();
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setBlendStateProperty(BlendState::Opaque);
        device.Clear(Color::White);
        const Matrix viewProjection = view * projection;
        lastReceiverSubmissions = 0;
        for (const Matrix& matrix : renderMatrices)
        {
            lastReceiverSubmissions += car.UseSelectionPlateShadow(
                *shadowEffect, matrix, viewProjection,
                lightViewProjection, textureScaleBias);
            lastReceiverSubmissions += car.UseShadow(
                *shadowEffect, matrix, viewProjection,
                lightViewProjection, textureScaleBias);
        }
        sceneMap.Resolve();
        FinishShadowPreparation();
    }

    void ShadowMapRenderer::FinishShadowPreparation()
    {
        auto& blurParameters = blurEffect->getParametersProperty();
        blurEffect->setCurrentTechniqueProperty(
            blurEffect->getTechniquesProperty()["ScreenAdvancedBlur20"]);
        if (EffectParameter* parameter = blurParameters["windowSize"])
            parameter->SetValue(Vector2(
                static_cast<float>(sceneMap.getWidthProperty()),
                static_cast<float>(sceneMap.getHeightProperty())));
        if (EffectParameter* parameter = blurParameters["sceneMap"])
            parameter->SetValue(&sceneMap.getXnaTextureProperty());
        blurMap.SetRenderTarget();
        device.setDepthStencilStateProperty(DepthStencilState::None);
        device.setBlendStateProperty(BlendState::Opaque);
        device.setRasterizerStateProperty(RasterizerState::CullNone);
        DrawFullscreenPass(0);
        blurMap.Resolve();
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.getSamplerStatesProperty()[0] = SamplerState::LinearWrap;
        device.setBlendStateProperty(BlendState::AlphaBlend);
        device.Clear(ClearOptions::DepthBuffer, Color::Black, 1.0f, 0);
    }

    void ShadowMapRenderer::DrawFullscreenPass(const int passIndex)
    {
        EffectTechnique* technique =
            blurEffect->getCurrentTechniqueProperty();
        technique->getPassesProperty()[passIndex].Apply();
        device.SetVertexBuffer(&fullscreenQuad);
        device.DrawPrimitives(PrimitiveType::TriangleStrip, 0, 2);
        device.SetVertexBuffer(nullptr);
    }

    void ShadowMapRenderer::ShowShadows()
    {
        auto& parameters = blurEffect->getParametersProperty();
        if (EffectParameter* parameter = parameters["blurMap"])
            parameter->SetValue(&blurMap.getXnaTextureProperty());
        blurEffect->setCurrentTechniqueProperty(
            blurEffect->getTechniquesProperty()["ScreenAdvancedBlur20"]);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        device.getSamplerStatesProperty()[0] = SamplerState::LinearClamp;
        device.setBlendStateProperty(zeroSourceBlend);
        device.setRasterizerStateProperty(RasterizerState::CullNone);
        DrawFullscreenPass(1);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.getSamplerStatesProperty()[0] = SamplerState::LinearWrap;
        device.setBlendStateProperty(BlendState::AlphaBlend);
        device.setRasterizerStateProperty(
            RasterizerState::CullCounterClockwise);
    }

    int ShadowMapRenderer::getLastCasterSubmissionCountProperty() const
    {
        return lastCasterSubmissions;
    }

    int ShadowMapRenderer::getLastReceiverSubmissionCountProperty() const
    {
        return lastReceiverSubmissions;
    }

    int ShadowMapRenderer::CountNonWhitePixels(const RenderTarget2D& target)
    {
        if (target.getFormatProperty() == SurfaceFormat::Color)
        {
            std::vector<Color> pixels(static_cast<std::size_t>(
                target.getWidthProperty() * target.getHeightProperty()));
            target.GetData(pixels.data(), static_cast<int>(pixels.size()));
            return static_cast<int>(std::ranges::count_if(
                pixels, [](const Color& pixel) { return pixel != Color::White; }));
        }

        using Microsoft::Xna::Framework::Graphics::PackedVector::Rgba64;
        std::vector<Rgba64> pixels(static_cast<std::size_t>(
            target.getWidthProperty() * target.getHeightProperty()));
        target.GetData(pixels.data(), static_cast<int>(pixels.size()));
        const Rgba64 white(1.0f, 1.0f, 1.0f, 1.0f);
        return static_cast<int>(std::ranges::count_if(
            pixels, [&](const Rgba64& pixel) { return pixel != white; }));
    }

    int ShadowMapRenderer::getShadowMapNonWhitePixelCountProperty() const
    {
        return CountNonWhitePixels(shadowMap.getRenderTargetProperty());
    }

    int ShadowMapRenderer::getReceiverMapNonWhitePixelCountProperty() const
    {
        return CountNonWhitePixels(sceneMap.getRenderTargetProperty());
    }
}
