// SPDX-License-Identifier: MS-PL

#include "Shaders/PostScreenMenu.hpp"

#include <stdexcept>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace RacingGame::Shaders
{
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using namespace Microsoft::Xna::Framework::Graphics;

    PostScreenMenu::PostScreenMenu(
        GraphicsDevice& device, ContentManager& content)
        : PostScreenMenu(
              device, content, "Shaders/PostScreenMenu", true)
    {
    }

    PostScreenMenu::PostScreenMenu(
        GraphicsDevice& setDevice, ContentManager& content,
        const std::string& effectAsset, const bool loadNoiseTexture)
        : device(setDevice),
          effect(content.Load<std::shared_ptr<Effect>>(effectAsset)),
          sceneMapTexture(std::make_unique<RenderToTexture>(
              device, RenderToTexture::SizeType::FullScreen)),
          downsampleMapTexture(std::make_unique<RenderToTexture>(
              device, RenderToTexture::SizeType::QuarterScreen)),
          blurMap1Texture(std::make_unique<RenderToTexture>(
              device, RenderToTexture::SizeType::QuarterScreen)),
          blurMap2Texture(std::make_unique<RenderToTexture>(
              device, RenderToTexture::SizeType::QuarterScreen)),
          screenHelper(device)
    {
        if (!effect)
            throw std::runtime_error(
                "Authentic Racing post-screen effect failed to load");
        auto& parameters = effect->getParametersProperty();
        if (!parameters["windowSize"] || !parameters["sceneMap"])
        {
            throw std::runtime_error(
                "windowSize and sceneMap must be valid in the Racing post-screen effect");
        }
        if (loadNoiseTexture)
        {
            noiseMapTexture.emplace(
                content.Load<Texture2D>("Textures/Noise128x128"));
            if (EffectParameter* parameter = parameters["noiseMap"])
                parameter->SetValue(&*noiseMapTexture);
        }
    }

    PostScreenMenu::~PostScreenMenu()
    {
        if (!started) return;
        try
        {
            RestoreAfterShow();
        }
        catch (...)
        {
            // Destructors must not leak a renderer cleanup failure.
        }
    }

    bool PostScreenMenu::getStartedProperty() const
    {
        return started;
    }

    bool PostScreenMenu::getEnabledProperty() const
    {
        return enabled;
    }

    void PostScreenMenu::setEnabledProperty(const bool value)
    {
        enabled = value;
    }

    int PostScreenMenu::getLastPassCountProperty() const
    {
        return lastPassCount;
    }

    void PostScreenMenu::Start()
    {
        if (!effect || started || !enabled) return;
        sceneMapTexture->SetRenderTarget();
        started = true;
    }

    void PostScreenMenu::SetWindowAndSceneParameters()
    {
        auto& parameters = effect->getParametersProperty();
        if (EffectParameter* parameter = parameters["windowSize"])
        {
            parameter->SetValue(Vector2(
                static_cast<float>(sceneMapTexture->getWidthProperty()),
                static_cast<float>(sceneMapTexture->getHeightProperty())));
        }
        if (EffectParameter* parameter = parameters["sceneMap"])
            parameter->SetValue(&sceneMapTexture->getXnaTextureProperty());
    }

    void PostScreenMenu::DrawPass(
        const int passIndex, const bool useGrid)
    {
        EffectTechnique* technique = effect->getCurrentTechniqueProperty();
        technique->getPassesProperty()[passIndex].Apply();
        if (useGrid)
            screenHelper.Render10x10Grid();
        else
            screenHelper.Render();
        ++lastPassCount;
    }

    void PostScreenMenu::RestoreAfterShow()
    {
        device.SetRenderTarget(static_cast<RenderTarget2D*>(nullptr));
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
    }

    void PostScreenMenu::Show(const float totalTimeSeconds)
    {
        lastPassCount = 0;
        if (!effect || !started) return;
        started = false;

        try
        {
            sceneMapTexture->Resolve();

            device.setDepthStencilStateProperty(DepthStencilState::None);
            device.setBlendStateProperty(BlendState::Opaque);
            SetWindowAndSceneParameters();
            auto& parameters = effect->getParametersProperty();
            if (EffectParameter* parameter = parameters["Timer"])
                parameter->SetValue(totalTimeSeconds + 0.75f);

            EffectTechnique* technique =
                effect->getTechniquesProperty()["ScreenGlow20"];
            if (!technique ||
                technique->getPassesProperty().getCountProperty() != 4)
            {
                throw std::runtime_error(
                    "Racing PostScreenMenu ScreenGlow20 must contain four passes");
            }
            effect->setCurrentTechniqueProperty(technique);

            for (int pass = 0; pass < 4; ++pass)
            {
                if (pass == 0)
                    downsampleMapTexture->SetRenderTarget();
                else if (pass == 1)
                    blurMap1Texture->SetRenderTarget();
                else if (pass == 2)
                    blurMap2Texture->SetRenderTarget();
                else
                    device.SetRenderTarget(static_cast<RenderTarget2D*>(nullptr));

                DrawPass(pass, false);
                if (pass == 0)
                {
                    downsampleMapTexture->Resolve();
                    if (EffectParameter* parameter = parameters["downsampleMap"])
                        parameter->SetValue(
                            &downsampleMapTexture->getXnaTextureProperty());
                    technique->getPassesProperty()[pass].Apply();
                }
                else if (pass == 1)
                {
                    blurMap1Texture->Resolve();
                    if (EffectParameter* parameter = parameters["blurMap1"])
                        parameter->SetValue(
                            &blurMap1Texture->getXnaTextureProperty());
                    technique->getPassesProperty()[pass].Apply();
                }
                else if (pass == 2)
                {
                    blurMap2Texture->Resolve();
                    if (EffectParameter* parameter = parameters["blurMap2"])
                        parameter->SetValue(
                            &blurMap2Texture->getXnaTextureProperty());
                    technique->getPassesProperty()[pass].Apply();
                }
            }
        }
        catch (...)
        {
            RestoreAfterShow();
            throw;
        }
        RestoreAfterShow();
    }
}
