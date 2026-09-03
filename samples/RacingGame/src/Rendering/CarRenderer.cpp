// SPDX-License-Identifier: MS-PL

#include "Rendering/CarRenderer.hpp"

#include <stdexcept>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace RacingGame::Rendering
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using namespace Microsoft::Xna::Framework::Graphics;

    CarRenderer::CarRenderer(
        GraphicsDevice& setDevice,
        Microsoft::Xna::Framework::Content::ContentManager& content)
        : device(setDevice),
          model(content.Load<Model>("Models/Car")),
          hierarchy(model),
          carTexture(content.Load<Texture2D>("Textures/RacerCar"))
    {
        InitializeEffects();
    }

    EffectTechnique* CarRenderer::SelectOriginalTechnique(
        Effect& effect, const SharpRuntime::String& meshName,
        const int meshPartNumber)
    {
        auto& techniques = effect.getTechniquesProperty();
        const int techniqueCount = techniques.getCountProperty();
        if (techniqueCount == 0)
            return nullptr;

        int techniqueIndex = -1;
        const std::size_t offset = static_cast<std::size_t>(meshPartNumber + 1);
        if (meshName.size() >= offset)
        {
            const char value = meshName[meshName.size() - offset];
            if (value >= '0' && value <= '9')
                techniqueIndex = value - '0';
        }
        if (techniqueIndex < 0 || techniqueIndex >= techniqueCount)
        {
            techniqueIndex = techniqueCount - 1;
            if (techniques[techniqueIndex].getNameProperty().find(
                    "SpecularWithReflection") != SharpRuntime::String::npos)
                techniqueIndex -= 2;
            if (techniqueIndex >= 0 &&
                techniques[techniqueIndex].getNameProperty().find(
                    "ReflectionSpecular") != SharpRuntime::String::npos)
                techniqueIndex -= 4;
        }
        return techniqueIndex >= 0 && techniqueIndex < techniqueCount
            ? &techniques[techniqueIndex]
            : nullptr;
    }

    void CarRenderer::InitializeEffects()
    {
        reflectionSpecularEffects.clear();
        for (ModelMesh* mesh : model.getMeshesProperty())
        {
            int meshPartNumber = 0;
            for (Effect* effect : mesh->getEffectsPropertyMutable())
            {
                if (!effect || !effect->getCurrentTechniqueProperty())
                    throw std::runtime_error(
                        "Authentic Racing car contains an incomplete effect");
                reflectionSpecularEffects.push_back(
                    effect->getCurrentTechniqueProperty()->getNameProperty().find(
                        "ReflectionSpecular") != SharpRuntime::String::npos);

                if (!mesh->getNameProperty().starts_with("glass"))
                {
                    if (EffectParameter* useAlpha =
                            effect->getParametersProperty()["UseAlpha"])
                        useAlpha->SetValue(false);
                }
                EffectTechnique* technique = SelectOriginalTechnique(
                    *effect, mesh->getNameProperty(), meshPartNumber++);
                if (!technique)
                    throw std::runtime_error(
                        "Authentic Racing car has no usable effect technique");
                effect->setCurrentTechniqueProperty(technique);
            }
        }
    }

    int CarRenderer::Draw(
        const float wheelPosition, const Matrix renderMatrix,
        const Matrix& view, const Matrix& projection,
        const Color carColor)
    {
        (void)carColor;
        const Matrix viewProjection = view * projection;
        const Matrix inverseView = Matrix::Invert(view);
        const Vector3 lightDirection = Vector3::Normalize(
            Vector3(8500.0f, -7250.0f, 15000.0f));
        const auto poses = hierarchy.BuildMeshPoses(
            wheelPosition, renderMatrix);
        int submittedParts = 0;

        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(
            RasterizerState::CullCounterClockwise);
        device.setBlendStateProperty(BlendState::Opaque);

        for (int alphaPass = 0; alphaPass < 2; ++alphaPass)
        {
            std::size_t effectIndex = 0;
            for (const Graphics::CarMeshPose& pose : poses)
            {
                bool skipMesh = false;
                for (Effect* effect : pose.mesh->getEffectsPropertyMutable())
                {
                    if (effectIndex >= reflectionSpecularEffects.size())
                        throw std::runtime_error(
                            "Authentic Racing car effect cache is inconsistent");
                    const bool reflection =
                        reflectionSpecularEffects[effectIndex++];
                    if ((alphaPass == 0 && reflection) ||
                        (alphaPass == 1 && !reflection))
                    {
                        skipMesh = true;
                        break;
                    }

                    auto& parameters = effect->getParametersProperty();
                    if (!reflection)
                    {
                        if (EffectParameter* parameter = parameters["diffuseTexture"])
                            parameter->SetValue(&carTexture);
                        if (EffectParameter* parameter = parameters["ambientColor"])
                            parameter->SetValue(Color(40, 40, 40).ToVector4());
                        if (EffectParameter* parameter = parameters["diffuseColor"])
                            parameter->SetValue(Color(210, 210, 210).ToVector4());
                    }
                    if (EffectParameter* parameter = parameters["world"])
                        parameter->SetValue(pose.world);
                    if (EffectParameter* parameter = parameters["viewProj"])
                        parameter->SetValue(viewProjection);
                    if (EffectParameter* parameter = parameters["viewInverse"])
                        parameter->SetValue(inverseView);
                    if (EffectParameter* parameter = parameters["lightDir"])
                        parameter->SetValue(lightDirection);
                }

                if (skipMesh)
                    continue;
                for (ModelMeshPart* part : pose.mesh->getMeshPartsProperty())
                {
                    Effect* effect = part->getEffectProperty();
                    if (!effect || !effect->getCurrentTechniqueProperty())
                        continue;
                    device.SetVertexBuffer(part->getVertexBufferProperty());
                    device.setIndicesProperty(part->getIndexBufferProperty());
                    for (EffectPass& pass :
                         effect->getCurrentTechniqueProperty()->getPassesProperty())
                    {
                        pass.Apply();
                        device.DrawIndexedPrimitives(
                            PrimitiveType::TriangleList,
                            part->getVertexOffsetProperty(), 0,
                            part->getNumVerticesProperty(),
                            part->getStartIndexProperty(),
                            part->getPrimitiveCountProperty());
                    }
                    ++submittedParts;
                }
            }
        }
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
        return submittedParts;
    }

    const Model& CarRenderer::getModelProperty() const
    {
        return model;
    }
}
