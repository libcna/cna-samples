// SPDX-License-Identifier: MS-PL

#include "Shaders/PostScreenGlow.hpp"

#include <stdexcept>

#include "GameLogic/CarPhysics.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Blend.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendFunction.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"

namespace RacingGame::Shaders
{
    using Microsoft::Xna::Framework::Content::ContentManager;
    using namespace Microsoft::Xna::Framework::Graphics;

    PostScreenGlow::PostScreenGlow(
        GraphicsDevice& device, ContentManager& content)
        : PostScreenMenu(
              device, content, "Shaders/PostScreenGlow", false),
          radialSceneMapTexture(std::make_unique<RenderToTexture>(
              device, RenderToTexture::SizeType::FullScreen)),
          screenBorderFadeoutMapTexture(
              content.Load<Texture2D>("Textures/ScreenBorderFadeout"))
    {
        auto& parameters = effect->getParametersProperty();
        if (!parameters["radialSceneMap"] ||
            !parameters["radialBlurScaleFactor"] ||
            !parameters["screenBorderFadeoutMap"])
        {
            throw std::runtime_error(
                "Authentic Racing PostScreenGlow parameters are incomplete");
        }
        parameters["screenBorderFadeoutMap"]->SetValue(
            &screenBorderFadeoutMapTexture);

        alphaWriteBlend.setColorSourceBlendProperty(Blend::One);
        alphaWriteBlend.setAlphaSourceBlendProperty(Blend::Zero);
        alphaWriteBlend.setColorDestinationBlendProperty(Blend::Zero);
        alphaWriteBlend.setAlphaDestinationBlendProperty(Blend::One);
        alphaWriteBlend.setColorBlendFunctionProperty(BlendFunction::Add);
        alphaWriteBlend.setAlphaBlendFunctionProperty(BlendFunction::Add);

        EffectTechnique* technique =
            effect->getTechniquesProperty()["ScreenGlow20"];
        if (!technique || technique->getPassesProperty().getCountProperty() != 5)
            throw std::runtime_error(
                "Racing PostScreenGlow ScreenGlow20 must contain five passes");
    }

    float PostScreenGlow::getRadialBlurScaleFactorProperty() const
    {
        return lastUsedRadialBlurScaleFactor;
    }

    void PostScreenGlow::setRadialBlurScaleFactorProperty(const float value)
    {
        if (lastUsedRadialBlurScaleFactor == value) return;
        lastUsedRadialBlurScaleFactor = value;
        effect->getParametersProperty()["radialBlurScaleFactor"]->SetValue(value);
    }

    void PostScreenGlow::Show(const float speed)
    {
        lastPassCount = 0;
        if (!effect || !started) return;
        started = false;

        try
        {
            sceneMapTexture->Resolve();

            device.setDepthStencilStateProperty(DepthStencilState::None);
            device.setBlendStateProperty(alphaWriteBlend);
            SetWindowAndSceneParameters();
            setRadialBlurScaleFactorProperty(
                -(0.0025f + speed * 0.005f /
                    GameLogic::CarPhysics::DefaultMaxSpeed));

            EffectTechnique* technique =
                effect->getTechniquesProperty()["ScreenGlow20"];
            if (!technique ||
                technique->getPassesProperty().getCountProperty() != 5)
            {
                throw std::runtime_error(
                    "Racing PostScreenGlow ScreenGlow20 must contain five passes");
            }
            effect->setCurrentTechniqueProperty(technique);
            auto& parameters = effect->getParametersProperty();

            for (int pass = 0; pass < 5; ++pass)
            {
                if (pass == 0)
                    radialSceneMapTexture->SetRenderTarget();
                else if (pass == 1)
                    downsampleMapTexture->SetRenderTarget();
                else if (pass == 2)
                    blurMap1Texture->SetRenderTarget();
                else if (pass == 3)
                    blurMap2Texture->SetRenderTarget();
                else
                    device.SetRenderTarget(static_cast<RenderTarget2D*>(nullptr));

                DrawPass(pass, pass == 0);
                if (pass == 0)
                {
                    radialSceneMapTexture->Resolve();
                    parameters["radialSceneMap"]->SetValue(
                        &radialSceneMapTexture->getXnaTextureProperty());
                    technique->getPassesProperty()[pass].Apply();
                }
                else if (pass == 1)
                {
                    downsampleMapTexture->Resolve();
                    if (EffectParameter* parameter = parameters["downsampleMap"])
                        parameter->SetValue(
                            &downsampleMapTexture->getXnaTextureProperty());
                    technique->getPassesProperty()[pass].Apply();
                }
                else if (pass == 2)
                {
                    blurMap1Texture->Resolve();
                    if (EffectParameter* parameter = parameters["blurMap1"])
                        parameter->SetValue(
                            &blurMap1Texture->getXnaTextureProperty());
                    technique->getPassesProperty()[pass].Apply();
                }
                else if (pass == 3)
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
