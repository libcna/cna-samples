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
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
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
          selectionPlate(content.Load<Model>("Models/CarSelectionPlate")),
          hierarchy(model),
          selectionPlateTransforms(static_cast<std::size_t>(
              selectionPlate.getBonesProperty().getCountProperty())),
          carTextures{
              content.Load<Texture2D>("Textures/RacerCar"),
              content.Load<Texture2D>("Textures/RacerCar2"),
              content.Load<Texture2D>("Textures/RacerCar3")},
          ghostEffect(content.Load<std::shared_ptr<Effect>>(
              "Shaders/LightingShader"))
    {
        if (!ghostEffect ||
            !ghostEffect->getTechniquesProperty()["ShadowCar"])
        {
            throw std::runtime_error(
                "Authentic Racing ShadowCar technique failed to load");
        }
        InitializeEffects();
        selectionPlate.CopyAbsoluteBoneTransformsTo(selectionPlateTransforms);
        InitializeSelectionPlateEffects();
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

    void CarRenderer::InitializeSelectionPlateEffects()
    {
        for (ModelMesh* mesh : selectionPlate.getMeshesProperty())
        {
            int partIndex = 0;
            for (Effect* effect : mesh->getEffectsPropertyMutable())
            {
                if (!effect)
                    throw std::runtime_error(
                        "Authentic Racing selection plate has no effect");
                auto& techniques = effect->getTechniquesProperty();
                const int count = techniques.getCountProperty();
                const std::size_t offset =
                    static_cast<std::size_t>(partIndex + 1);
                int techniqueIndex = 0;
                if (mesh->getNameProperty().size() >= offset)
                {
                    const char digit = mesh->getNameProperty()[
                        mesh->getNameProperty().size() - offset];
                    techniqueIndex = digit >= '0' && digit <= '9'
                        ? digit - '0' : 0;
                }
                if (techniqueIndex < 0 || techniqueIndex >= count)
                {
                    techniqueIndex = count - 1;
                    if (techniques[techniqueIndex].getNameProperty().find(
                            "SpecularWithReflection") != std::string::npos)
                        techniqueIndex -= 2;
                    if (techniqueIndex >= 0 &&
                        techniques[techniqueIndex].getNameProperty().find(
                            "ReflectionSpecular") != std::string::npos)
                        techniqueIndex -= 4;
                }
                if (techniqueIndex < 0 || techniqueIndex >= count)
                    throw std::runtime_error(
                        "Authentic Racing selection plate selected an invalid technique");
                effect->setCurrentTechniqueProperty(
                    &techniques[techniqueIndex]);
                ++partIndex;
            }
        }
    }

    int CarRenderer::Draw(
        const int carNumber, const float wheelPosition,
        const Matrix renderMatrix,
        const Matrix& view, const Matrix& projection,
        const Color carColor)
    {
        if (carNumber < 0)
            throw std::out_of_range("Racing car number cannot be negative");
        Texture2D& carTexture = carTextures[
            static_cast<std::size_t>(carNumber) % carTextures.size()];
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
                int effectNumber = 0;
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
                        if (carColor != Color::White && effectNumber == 0)
                        {
                            if (EffectTechnique* colorTechnique =
                                    effect->getTechniquesProperty()[
                                        "SpecularWithReflectionForCar20"])
                                effect->setCurrentTechniqueProperty(colorTechnique);
                            if (EffectParameter* parameter =
                                    parameters["carHueColor"])
                                parameter->SetValue(carColor.ToVector3());
                        }
                    }
                    if (EffectParameter* parameter = parameters["world"])
                        parameter->SetValue(pose.world);
                    if (EffectParameter* parameter = parameters["viewProj"])
                        parameter->SetValue(viewProjection);
                    if (EffectParameter* parameter = parameters["viewInverse"])
                        parameter->SetValue(inverseView);
                    if (EffectParameter* parameter = parameters["lightDir"])
                        parameter->SetValue(lightDirection);
                    ++effectNumber;
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
                if (carColor != Color::White)
                {
                    int meshPartNumber = 0;
                    for (Effect* effect :
                         pose.mesh->getEffectsPropertyMutable())
                    {
                        if (EffectTechnique* technique = SelectOriginalTechnique(
                                *effect, pose.mesh->getNameProperty(),
                                meshPartNumber++))
                            effect->setCurrentTechniqueProperty(technique);
                    }
                }
            }
        }
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
        return submittedParts;
    }

    int CarRenderer::DrawGhost(
        const float wheelPosition, const Matrix renderMatrix,
        const Matrix& view, const Matrix& projection)
    {
        EffectTechnique* technique =
            ghostEffect->getTechniquesProperty()["ShadowCar"];
        if (!technique)
            throw std::runtime_error("Missing authentic ShadowCar technique");
        ghostEffect->setCurrentTechniqueProperty(technique);

        const Matrix viewProjection = view * projection;
        const Matrix inverseView = Matrix::Invert(view);
        const Vector3 lightDirection = Vector3::Normalize(
            Vector3(8500.0f, -7250.0f, 15000.0f));
        const auto poses = hierarchy.BuildMeshPoses(
            wheelPosition, renderMatrix);
        int submittedParts = 0;

        for (const Graphics::CarMeshPose& pose : poses)
        {
            auto& parameters = ghostEffect->getParametersProperty();
            if (EffectParameter* parameter = parameters["world"])
                parameter->SetValue(pose.world);
            if (EffectParameter* parameter = parameters["worldViewProj"])
                parameter->SetValue(pose.world * viewProjection);
            if (EffectParameter* parameter = parameters["viewProj"])
                parameter->SetValue(viewProjection);
            if (EffectParameter* parameter = parameters["viewInverse"])
                parameter->SetValue(inverseView);
            if (EffectParameter* parameter = parameters["lightDir"])
                parameter->SetValue(lightDirection);

            for (ModelMeshPart* part : pose.mesh->getMeshPartsProperty())
            {
                device.SetVertexBuffer(part->getVertexBufferProperty());
                device.setIndicesProperty(part->getIndexBufferProperty());
                for (EffectPass& pass : technique->getPassesProperty())
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
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
        return submittedParts;
    }

    int CarRenderer::DrawSelectionPlate(
        Matrix renderMatrix, const Matrix& view, const Matrix& projection)
    {
        const Matrix viewProjection = view * projection;
        const Matrix inverseView = Matrix::Invert(view);
        const Vector3 lightDirection = Vector3::Normalize(
            Vector3(-8500.0f, 7250.0f, 15000.0f));
        renderMatrix = Graphics::CarModelHierarchy::GetObjectMatrix() *
                       renderMatrix;
        int submissions = 0;
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(
            RasterizerState::CullCounterClockwise);
        device.setBlendStateProperty(BlendState::Opaque);
        for (ModelMesh* mesh : selectionPlate.getMeshesProperty())
        {
            const Matrix world = selectionPlateTransforms.at(
                static_cast<std::size_t>(
                    mesh->getParentBoneProperty()->getIndexProperty())) *
                renderMatrix;
            for (Effect* effect : mesh->getEffectsPropertyMutable())
            {
                auto& parameters = effect->getParametersProperty();
                if (EffectParameter* parameter = parameters["world"])
                    parameter->SetValue(world);
                if (EffectParameter* parameter = parameters["viewProj"])
                    parameter->SetValue(viewProjection);
                if (EffectParameter* parameter = parameters["viewInverse"])
                    parameter->SetValue(inverseView);
                if (EffectParameter* parameter = parameters["lightDir"])
                    parameter->SetValue(lightDirection);
            }
            for (ModelMeshPart* part : mesh->getMeshPartsProperty())
            {
                Effect* effect = part->getEffectProperty();
                if (!effect || !effect->getCurrentTechniqueProperty()) continue;
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
                ++submissions;
            }
        }
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
        return submissions;
    }

    int CarRenderer::UseSelectionPlateShadow(
        Effect& effect, const Matrix renderMatrix,
        const Matrix& viewProjection, const Matrix& lightViewProjection,
        const Matrix& textureScaleBias)
    {
        return DrawSelectionPlateShadowParts(
            effect, renderMatrix, viewProjection, lightViewProjection,
            &textureScaleBias);
    }

    int CarRenderer::GenerateShadow(
        Effect& effect, const Matrix renderMatrix,
        const Matrix& lightViewProjection)
    {
        return DrawShadowParts(effect, renderMatrix, lightViewProjection,
                               lightViewProjection, nullptr);
    }

    int CarRenderer::UseShadow(
        Effect& effect, const Matrix renderMatrix,
        const Matrix& viewProjection, const Matrix& lightViewProjection,
        const Matrix& textureScaleBias)
    {
        return DrawShadowParts(effect, renderMatrix, viewProjection,
                               lightViewProjection, &textureScaleBias);
    }

    int CarRenderer::DrawShadowParts(
        Effect& effect, const Matrix renderMatrix,
        const Matrix& viewProjection, const Matrix& lightViewProjection,
        const Matrix* textureScaleBias)
    {
        const auto poses = hierarchy.BuildMeshPoses(0.0f, renderMatrix);
        auto& parameters = effect.getParametersProperty();
        int submissions = 0;
        for (const Graphics::CarMeshPose& pose : poses)
        {
            if (EffectParameter* parameter = parameters["world"])
                parameter->SetValue(pose.world);
            if (EffectParameter* parameter = parameters["worldViewProj"])
                parameter->SetValue(pose.world * viewProjection);
            if (EffectParameter* parameter = parameters["worldViewProjLight"])
                parameter->SetValue(pose.world * lightViewProjection);
            if (textureScaleBias)
            {
                if (EffectParameter* parameter =
                        parameters["shadowTexTransform"])
                    parameter->SetValue(
                        pose.world * lightViewProjection * *textureScaleBias);
            }
            effect.getCurrentTechniqueProperty()
                ->getPassesProperty()[0]
                .Apply();
            for (ModelMeshPart* part : pose.mesh->getMeshPartsProperty())
            {
                device.SetVertexBuffer(part->getVertexBufferProperty());
                device.setIndicesProperty(part->getIndexBufferProperty());
                device.DrawIndexedPrimitives(
                    PrimitiveType::TriangleList,
                    part->getVertexOffsetProperty(), 0,
                    part->getNumVerticesProperty(),
                    part->getStartIndexProperty(),
                    part->getPrimitiveCountProperty());
                ++submissions;
            }
        }
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
        return submissions;
    }

    int CarRenderer::DrawSelectionPlateShadowParts(
        Effect& effect, Matrix renderMatrix,
        const Matrix& viewProjection, const Matrix& lightViewProjection,
        const Matrix* textureScaleBias)
    {
        renderMatrix = Graphics::CarModelHierarchy::GetObjectMatrix() *
                       renderMatrix;
        auto& parameters = effect.getParametersProperty();
        int submissions = 0;
        for (ModelMesh* mesh : selectionPlate.getMeshesProperty())
        {
            const Matrix world = selectionPlateTransforms.at(
                static_cast<std::size_t>(
                    mesh->getParentBoneProperty()->getIndexProperty())) *
                renderMatrix;
            if (EffectParameter* parameter = parameters["world"])
                parameter->SetValue(world);
            if (EffectParameter* parameter = parameters["worldViewProj"])
                parameter->SetValue(world * viewProjection);
            if (EffectParameter* parameter = parameters["worldViewProjLight"])
                parameter->SetValue(world * lightViewProjection);
            if (textureScaleBias)
            {
                if (EffectParameter* parameter =
                        parameters["shadowTexTransform"])
                    parameter->SetValue(
                        world * lightViewProjection * *textureScaleBias);
            }
            effect.getCurrentTechniqueProperty()
                ->getPassesProperty()[0].Apply();
            for (ModelMeshPart* part : mesh->getMeshPartsProperty())
            {
                device.SetVertexBuffer(part->getVertexBufferProperty());
                device.setIndicesProperty(part->getIndexBufferProperty());
                device.DrawIndexedPrimitives(
                    PrimitiveType::TriangleList,
                    part->getVertexOffsetProperty(), 0,
                    part->getNumVerticesProperty(),
                    part->getStartIndexProperty(),
                    part->getPrimitiveCountProperty());
                ++submissions;
            }
        }
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
        return submissions;
    }

    const Model& CarRenderer::getModelProperty() const
    {
        return model;
    }
}
