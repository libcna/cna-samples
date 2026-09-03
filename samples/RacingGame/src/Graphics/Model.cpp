// SPDX-License-Identifier: MS-PL

#include "Graphics/Model.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

#include "Helpers/Vector3Helper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelBone.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace RacingGame::Graphics
{
    using Helpers::Vector3Helper;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::MathHelper;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using namespace Microsoft::Xna::Framework::Graphics;

    int Model::maxViewDistance = 200;

    Model::Model(
        const std::string& setModelName,
        Microsoft::Xna::Framework::Content::ContentManager& content,
        MeshRenderManager& meshManager)
        : name(setModelName),
          xnaModel(content.Load<Microsoft::Xna::Framework::Graphics::Model>(
              "Models/" + name))
    {
        const int boneCount = xnaModel.getBonesProperty().getCountProperty();
        transforms.resize(static_cast<std::size_t>(boneCount));
        xnaModel.CopyAbsoluteBoneTransformsTo(transforms);

        const auto& meshes = xnaModel.getMeshesProperty();
        if (meshes.getCountProperty() == 0)
            throw std::invalid_argument(
                "Invalid Racing model without meshes: " + name);
        const ModelMesh* firstMesh = meshes[0];
        realScaling = scaling =
            firstMesh->getBoundingSphereProperty().Radius *
            transforms[0].getRightProperty().Length();

        const std::string lowerName = ToLower(name);
        hasAlpha = lowerName.starts_with("alpha");
        if (lowerName == "alphapalm" || lowerName == "alphapalm2" ||
            lowerName == "alphapalm3" || lowerName == "roadcolumnsegment")
            scaling *= 0.75f;
        if (lowerName == "hotel01" || lowerName == "hotel02" ||
            lowerName == "casino01" || lowerName == "windmill")
            scaling *= 5.0f;
        else if (scaling > 3.0f)
            scaling = 3.0f;

        const bool isSign = lowerName.starts_with("sign") ||
                            lowerName.starts_with("banner") ||
                            lowerName.starts_with("windmill");
        for (ModelMesh* mesh : meshes)
        {
            if (lowerName == "windmill" &&
                ToLower(mesh->getNameProperty()).starts_with("windmill_wings"))
                animatedMesh = mesh;

            MeshEntry entry;
            entry.mesh = mesh;
            const auto& parts = mesh->getMeshPartsProperty();
            entry.renderables.reserve(
                static_cast<std::size_t>(parts.getCountProperty()));
            for (int partIndex = 0; partIndex < parts.getCountProperty();
                 ++partIndex)
            {
                ModelMeshPart* part = parts[partIndex];
                Effect* effect = part->getEffectProperty();
                if (!effect)
                    throw std::runtime_error(
                        "Racing model part has no processed effect: " + name);
                if (isSign)
                {
                    if (EffectParameter* ambient =
                            effect->getParametersProperty()["ambientColor"])
                        ambient->SetValue(Color(128, 128, 128).ToVector4());
                }
                const std::string technique = SelectTechniqueName(
                    *effect, mesh->getNameProperty(), partIndex);
                effect->setCurrentTechniqueProperty(
                    effect->getTechniquesProperty()[technique]);
                entry.renderables.push_back(
                    meshManager.Add(*part, *effect, technique));
                ++meshPartCount;
            }
            meshEntries.push_back(std::move(entry));
        }
    }

    const std::string& Model::getNameProperty() const
    {
        return name;
    }

    float Model::getSizeProperty() const
    {
        return realScaling;
    }

    int Model::getNumOfMeshPartsProperty() const
    {
        return meshPartCount;
    }

    int Model::getMaxViewDistanceProperty()
    {
        return maxViewDistance;
    }

    void Model::setMaxViewDistanceProperty(const int value)
    {
        if (value < maxViewDistance)
            maxViewDistance = value;
    }

    void Model::Render(Matrix renderMatrix, const Vector3 cameraPosition,
                       const Vector3 cameraRotation,
                       const float totalTimeSeconds)
    {
        const float maxDistance = maxViewDistance * scaling;
        const float distanceSquared = Vector3::DistanceSquared(
            cameraPosition, renderMatrix.getTranslationProperty());
        if (distanceSquared > maxDistance * maxDistance) return;

        if (distanceSquared > 20.0f * 20.0f &&
            distanceSquared > (10.0f * scaling) * (10.0f * scaling))
        {
            const Vector3 objectDirection = Vector3::Normalize(
                cameraPosition - renderMatrix.getTranslationProperty());
            constexpr float ViewableFieldOfView =
                (MathHelper::Pi / 2.0f) / 1.125f;
            if (Vector3Helper::GetAngleBetweenVectors(
                    cameraRotation, objectDirection) > ViewableFieldOfView)
                return;
        }

        renderMatrix = Matrix::CreateRotationX(MathHelper::Pi / 2.0f) *
                       renderMatrix;
        for (MeshEntry& entry : meshEntries)
        {
            const Matrix world = GetWorldMatrix(
                entry, renderMatrix, totalTimeSeconds);
            for (MeshRenderManager::RenderableMesh* renderable :
                 entry.renderables)
                renderable->AddRenderMatrix(world);
        }
    }

    int Model::GenerateShadow(
        GraphicsDevice& device, Matrix renderMatrix, Effect& effect,
        const Matrix& lightViewProjection, const Vector3 shadowLightPosition,
        const float shadowDistance, const float totalTimeSeconds)
    {
        const float maximumDistance =
            scaling / 2.5f + 1.015f * shadowDistance;
        if (Vector3::DistanceSquared(
                shadowLightPosition,
                renderMatrix.getTranslationProperty()) >
            maximumDistance * maximumDistance)
            return 0;
        renderMatrix = Matrix::CreateRotationX(MathHelper::Pi / 2.0f) *
                       renderMatrix;
        return DrawShadowParts(device, effect, renderMatrix, lightViewProjection,
                               lightViewProjection, nullptr,
                               totalTimeSeconds);
    }

    int Model::UseShadow(
        GraphicsDevice& device, Matrix renderMatrix, Effect& effect,
        const Matrix& viewProjection,
        const Matrix& lightViewProjection, const Matrix& textureScaleBias,
        const Vector3 shadowLightPosition, const float shadowDistance,
        const float totalTimeSeconds)
    {
        if (hasAlpha) return 0;
        const float maximumDistance = 1.015f * shadowDistance;
        if (Vector3::DistanceSquared(
                shadowLightPosition,
                renderMatrix.getTranslationProperty()) >
            maximumDistance * maximumDistance)
            return 0;
        renderMatrix = Matrix::CreateRotationX(MathHelper::Pi / 2.0f) *
                       renderMatrix;
        return DrawShadowParts(device, effect, renderMatrix, viewProjection,
                               lightViewProjection, &textureScaleBias,
                               totalTimeSeconds);
    }

    Matrix Model::GetWorldMatrix(
        const MeshEntry& entry, const Matrix renderMatrix,
        const float totalTimeSeconds) const
    {
        const ModelBone* parent = entry.mesh->getParentBoneProperty();
        const int boneIndex = parent ? parent->getIndexProperty() : 0;
        Matrix world = transforms[static_cast<std::size_t>(boneIndex)] *
                       renderMatrix;
        if (entry.mesh == animatedMesh)
        {
            world = Matrix::CreateRotationZ(
                        renderMatrix.getTranslationProperty().Length() * 3.0f +
                        renderMatrix.Determinant() * 5.0f +
                        (1.0f +
                         (static_cast<int>(renderMatrix.M42 * 33.3f) % 100) *
                             0.00123f) *
                            totalTimeSeconds / 0.654f) *
                    transforms[static_cast<std::size_t>(boneIndex)] *
                    renderMatrix;
        }
        return world;
    }

    int Model::DrawShadowParts(
        GraphicsDevice& device, Effect& effect, const Matrix renderMatrix,
        const Matrix& viewProjection, const Matrix& lightViewProjection,
        const Matrix* textureScaleBias, const float totalTimeSeconds)
    {
        auto& parameters = effect.getParametersProperty();
        int submissions = 0;
        for (const MeshEntry& entry : meshEntries)
        {
            const Matrix world = GetWorldMatrix(
                entry, renderMatrix, totalTimeSeconds);
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
                ->getPassesProperty()[0]
                .Apply();
            for (ModelMeshPart* part : entry.mesh->getMeshPartsProperty())
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
        return submissions;
    }

    std::string Model::ToLower(std::string value)
    {
        std::ranges::transform(value, value.begin(), [](const unsigned char c)
        {
            return static_cast<char>(std::tolower(c));
        });
        return value;
    }

    std::string Model::SelectTechniqueName(
        Effect& effect, const std::string& meshName, const int partIndex)
    {
        auto& techniques = effect.getTechniquesProperty();
        const int count = techniques.getCountProperty();
        if (count <= 0)
            throw std::runtime_error("Racing model effect has no techniques");

        int techniqueIndex = -1;
        const std::size_t offset = static_cast<std::size_t>(partIndex + 1);
        if (meshName.size() >= offset)
        {
            const char digit = meshName[meshName.size() - offset];
            // Int32.TryParse assigns zero to its out parameter when parsing fails.
            techniqueIndex = digit >= '0' && digit <= '9' ? digit - '0' : 0;
        }
        if (techniqueIndex < 0 || techniqueIndex >= count)
        {
            techniqueIndex = count - 1;
            if (techniques[techniqueIndex].getNameProperty().contains(
                    "SpecularWithReflection"))
                techniqueIndex -= 2;
            if (techniqueIndex >= 0 &&
                techniques[techniqueIndex].getNameProperty().contains(
                    "ReflectionSpecular"))
                techniqueIndex -= 4;
        }
        if (techniqueIndex < 0 || techniqueIndex >= count)
            throw std::runtime_error(
                "Racing mesh selected an invalid effect technique");
        return techniques[techniqueIndex].getNameProperty();
    }
}
