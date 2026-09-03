// SPDX-License-Identifier: MS-PL

#include "Rendering/StaticTrackScene.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <stdexcept>

#include "GameLogic/CarPhysics.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexElementSize.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"
#include "Rendering/LandscapeObjectRenderer.hpp"

namespace RacingGame::Rendering
{
    using Graphics::TangentVertex;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector2;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Vector4;
    using namespace Microsoft::Xna::Framework::Graphics;

    StaticTrackScene::StaticTrackScene(
        GraphicsDevice& setDevice,
        Microsoft::Xna::Framework::Content::ContentManager& content,
        const SharpRuntime::String& trackName,
        const bool setHighDetail)
        : device(setDevice),
          landscape(),
          track(trackName, landscape),
          trackGeometry(track),
          leftGuard(track, Tracks::GuardRailGeometry::Mode::Left),
          rightGuard(track, Tracks::GuardRailGeometry::Mode::Right),
          columns(track, landscape),
          highDetail(setHighDetail)
    {
        landscapeMesh = Upload(landscape.getVerticesProperty(),
                               landscape.getIndicesProperty());
        roadMesh = Upload(trackGeometry.getRoadVerticesProperty(),
                          trackGeometry.getRoadIndicesProperty());
        roadBackMesh = Upload(trackGeometry.getRoadBackVerticesProperty(),
                              trackGeometry.getRoadBackIndicesProperty());
        tunnelMesh = Upload(trackGeometry.getRoadTunnelVerticesProperty(),
                            trackGeometry.getRoadTunnelIndicesProperty());
        leftGuardMesh = Upload(leftGuard.getVerticesProperty(),
                               leftGuard.getIndicesProperty());
        rightGuardMesh = Upload(rightGuard.getVerticesProperty(),
                                rightGuard.getIndicesProperty());
        columnsMesh = Upload(columns.getVerticesProperty(),
                             columns.getIndicesProperty());

        normalEffect = content.Load<std::shared_ptr<Effect>>(
            "Shaders/NormalMapping");
        brakeTrackEffect = content.Load<std::shared_ptr<Effect>>(
            "Shaders/LightingShader");
        landscapeEffect = content.Load<std::shared_ptr<Effect>>(
            "Shaders/LandscapeNormalMapping");
        skyEffect = content.Load<std::shared_ptr<Effect>>(
            "Shaders/PreScreenSkyCubeMapping");
        skyCubeModel.emplace(content.Load<Model>("Models/Cube"));
        skyCubeTexture.emplace(content.Load<TextureCube>("Textures/SkyCubeMap"));
        landscapeDiffuse.emplace(content.Load<Texture2D>("Textures/Landscape"));
        landscapeNormal.emplace(content.Load<Texture2D>("Textures/LandscapeNormal"));
        landscapeDetail.emplace(content.Load<Texture2D>("Textures/LandscapeDetail"));
        cityDiffuse.emplace(content.Load<Texture2D>("Textures/CityGround"));
        cityNormal.emplace(content.Load<Texture2D>("Textures/CityGroundNormal"));
        brakeTrackDiffuse.emplace(content.Load<Texture2D>("Textures/track"));
        roadDiffuse.emplace(content.Load<Texture2D>("Textures/Road"));
        roadNormal.emplace(content.Load<Texture2D>("Textures/RoadNormal"));
        roadBackDiffuse.emplace(content.Load<Texture2D>("Textures/RoadBack"));
        roadBackNormal.emplace(content.Load<Texture2D>("Textures/RoadBackNormal"));
        tunnelDiffuse.emplace(content.Load<Texture2D>("Textures/RoadTunnel"));
        tunnelNormal.emplace(content.Load<Texture2D>("Textures/RoadTunnelNormal"));
        guardDiffuse.emplace(content.Load<Texture2D>("Textures/Leitplanke"));
        guardNormal.emplace(content.Load<Texture2D>("Textures/LeitplankeNormal"));
        columnDiffuse.emplace(content.Load<Texture2D>("Textures/RoadCement"));
        columnNormal.emplace(content.Load<Texture2D>("Textures/RoadCementNormal"));
        if (!normalEffect || !brakeTrackEffect || !landscapeEffect ||
            !skyEffect)
            throw std::runtime_error("Racing static scene effects failed to load");
        landscapeObjects = std::make_unique<LandscapeObjectRenderer>(
            device, content, landscape, track,
            leftGuard.getHolderMatricesProperty(),
            rightGuard.getHolderMatricesProperty(),
            columns.getSegmentPositionsProperty(), highDetail);
        cityPlaneAnchor = landscapeObjects->getCityPlaneAnchorProperty();
        if (cityPlaneAnchor)
        {
            const Vector3 up = Vector3::UnitZ;
            const Vector3 helper = Vector3::Cross(up, Vector3::UnitX);
            const Vector3 right = Vector3::Cross(helper, up);
            const Vector3 direction = Vector3::Cross(up, right);
            const float size = std::min(cityPlaneAnchor->X,
                                        cityPlaneAnchor->Y);
            constexpr float tiling = 20.0f;
            constexpr float distance = 0.1f;
            const std::vector<TangentVertex> vertices = {
                {(-right - direction) * size + up * distance,
                 Vector2(-size / tiling, -size / tiling), up, right},
                {(-right + direction) * size + up * distance,
                 Vector2(-size / tiling, size / tiling), up, right},
                {(right - direction) * size + up * distance,
                 Vector2(size / tiling, -size / tiling), up, right},
                {(right + direction) * size + up * distance,
                 Vector2(size / tiling, size / tiling), up, right},
            };
            cityPlaneMesh = Upload(
                vertices, std::vector<std::uint32_t>{0, 1, 2, 2, 1, 3});
        }
    }

    StaticTrackScene::~StaticTrackScene() = default;

    StaticTrackScene::GpuMesh StaticTrackScene::Upload(
        const std::vector<TangentVertex>& vertices,
        const std::vector<std::int32_t>& indices)
    {
        std::vector<std::uint32_t> unsignedIndices;
        unsignedIndices.reserve(indices.size());
        for (const std::int32_t index : indices)
        {
            if (index < 0)
                throw std::runtime_error("Racing geometry contains a negative index");
            unsignedIndices.push_back(static_cast<std::uint32_t>(index));
        }
        return Upload(vertices, unsignedIndices);
    }

    StaticTrackScene::GpuMesh StaticTrackScene::Upload(
        const std::vector<TangentVertex>& vertices,
        const std::vector<std::uint32_t>& indices)
    {
        GpuMesh mesh;
        mesh.vertexCount = static_cast<int>(vertices.size());
        mesh.primitiveCount = static_cast<int>(indices.size() / 3);
        if (vertices.empty() || indices.empty()) return mesh;
        mesh.vertexBuffer = std::make_unique<VertexBuffer>(
            device, TangentVertex::getVertexDeclarationStatic(),
            mesh.vertexCount, BufferUsage::WriteOnly);
        mesh.vertexBuffer->SetData(vertices.data(), mesh.vertexCount);
        mesh.indexBuffer = std::make_unique<IndexBuffer>(
            device, IndexElementSize::ThirtyTwoBits,
            static_cast<int>(indices.size()), BufferUsage::WriteOnly);
        mesh.indexBuffer->SetData(indices.data(), static_cast<int>(indices.size()));
        return mesh;
    }

    void StaticTrackScene::SetCommonParameters(
        Effect& effect, const Matrix& view, const Matrix& projection,
        const Matrix& world)
    {
        const Matrix viewProjection = view * projection;
        auto& parameters = effect.getParametersProperty();
        if (EffectParameter* parameter = parameters["world"])
            parameter->SetValue(world);
        if (EffectParameter* parameter = parameters["worldViewProj"])
            parameter->SetValue(world * viewProjection);
        if (EffectParameter* parameter = parameters["viewProj"])
            parameter->SetValue(viewProjection);
        if (EffectParameter* parameter = parameters["viewInverse"])
            parameter->SetValue(Matrix::Invert(view));
        if (EffectParameter* parameter = parameters["lightDir"])
            parameter->SetValue(Vector3::Normalize(
                Vector3(8500.0f, -7250.0f, 15000.0f)));
        if (EffectParameter* parameter = parameters["reflectionCubeTexture"])
            parameter->SetValue(&*skyCubeTexture);
    }

    void StaticTrackScene::SetMaterialParameters(
        Effect& effect, Texture2D& diffuse, Texture2D& normal,
        const Color& ambientColor, const Color& diffuseColor,
        const Color& specularColor)
    {
        auto& parameters = effect.getParametersProperty();
        if (EffectParameter* parameter = parameters["ambientColor"])
            parameter->SetValue(ambientColor.ToVector4());
        if (EffectParameter* parameter = parameters["diffuseColor"])
            parameter->SetValue(diffuseColor.ToVector4());
        if (EffectParameter* parameter = parameters["specularColor"])
            parameter->SetValue(specularColor.ToVector4());
        if (EffectParameter* parameter = parameters["shininess"])
            parameter->SetValue(24.0f);
        if (EffectParameter* parameter = parameters["UseAlpha"])
            parameter->SetValue(false);
        if (EffectParameter* parameter = parameters["diffuseTexture"])
            parameter->SetValue(&diffuse);
        if (EffectParameter* parameter = parameters["normalTexture"])
            parameter->SetValue(&normal);
    }

    void StaticTrackScene::DrawMesh(GpuMesh& mesh, Effect& effect,
                                    const SharpRuntime::String& techniqueName)
    {
        if (!mesh.vertexBuffer || !mesh.indexBuffer) return;
        EffectTechnique* technique = effect.getTechniquesProperty()[techniqueName];
        if (!technique)
            throw std::runtime_error("Missing Racing effect technique: " +
                                     techniqueName);
        effect.setCurrentTechniqueProperty(technique);
        device.SetVertexBuffer(mesh.vertexBuffer.get());
        device.setIndicesProperty(mesh.indexBuffer.get());
        for (EffectPass& pass : technique->getPassesProperty())
        {
            pass.Apply();
            device.DrawIndexedPrimitives(
                PrimitiveType::TriangleList, 0, 0, mesh.vertexCount,
                0, mesh.primitiveCount);
        }
    }

    int StaticTrackScene::DrawShadowMesh(
        GpuMesh& mesh, Effect& effect, const Matrix& viewProjection,
        const Matrix& lightViewProjection, const Matrix* textureScaleBias)
    {
        if (!mesh.vertexBuffer || !mesh.indexBuffer) return 0;
        const Matrix world = Matrix::getIdentityProperty();
        auto& parameters = effect.getParametersProperty();
        if (EffectParameter* parameter = parameters["world"])
            parameter->SetValue(world);
        if (EffectParameter* parameter = parameters["worldViewProj"])
            parameter->SetValue(world * viewProjection);
        if (EffectParameter* parameter = parameters["worldViewProjLight"])
            parameter->SetValue(world * lightViewProjection);
        if (textureScaleBias)
        {
            if (EffectParameter* parameter = parameters["shadowTexTransform"])
                parameter->SetValue(
                    world * lightViewProjection * *textureScaleBias);
        }
        effect.getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();
        device.SetVertexBuffer(mesh.vertexBuffer.get());
        device.setIndicesProperty(mesh.indexBuffer.get());
        device.DrawIndexedPrimitives(
            PrimitiveType::TriangleList, 0, 0, mesh.vertexCount,
            0, mesh.primitiveCount);
        return 1;
    }

    int StaticTrackScene::GenerateShadows(
        Effect& effect, const Matrix& lightViewProjection,
        const Vector3 shadowLightPosition, const float shadowDistance,
        const float totalTimeSeconds)
    {
        int submissions = 0;
        device.setRasterizerStateProperty(RasterizerState::CullNone);
        submissions += DrawShadowMesh(
            roadMesh, effect, lightViewProjection, lightViewProjection,
            nullptr);
        submissions += DrawShadowMesh(
            tunnelMesh, effect, lightViewProjection, lightViewProjection,
            nullptr);
        device.setRasterizerStateProperty(
            RasterizerState::CullCounterClockwise);
        submissions += DrawShadowMesh(
            leftGuardMesh, effect, lightViewProjection, lightViewProjection,
            nullptr);
        submissions += DrawShadowMesh(
            rightGuardMesh, effect, lightViewProjection, lightViewProjection,
            nullptr);
        submissions += landscapeObjects->GenerateShadows(
            effect, lightViewProjection, shadowLightPosition,
            shadowDistance, totalTimeSeconds);
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
        return submissions;
    }

    int StaticTrackScene::UseShadows(
        Effect& effect, const Matrix& viewProjection,
        const Matrix& lightViewProjection, const Matrix& textureScaleBias,
        const Vector3 shadowLightPosition, const float shadowDistance,
        const float totalTimeSeconds)
    {
        int submissions = 0;
        device.setRasterizerStateProperty(
            RasterizerState::CullCounterClockwise);
        submissions += DrawShadowMesh(
            landscapeMesh, effect, viewProjection, lightViewProjection,
            &textureScaleBias);
        submissions += landscapeObjects->UseShadows(
            effect, viewProjection, lightViewProjection, textureScaleBias,
            shadowLightPosition, shadowDistance, totalTimeSeconds);
        submissions += DrawShadowMesh(
            roadMesh, effect, viewProjection, lightViewProjection,
            &textureScaleBias);
        device.setRasterizerStateProperty(RasterizerState::CullNone);
        submissions += DrawShadowMesh(
            tunnelMesh, effect, viewProjection, lightViewProjection,
            &textureScaleBias);
        device.setRasterizerStateProperty(
            RasterizerState::CullCounterClockwise);
        submissions += DrawShadowMesh(
            leftGuardMesh, effect, viewProjection, lightViewProjection,
            &textureScaleBias);
        submissions += DrawShadowMesh(
            rightGuardMesh, effect, viewProjection, lightViewProjection,
            &textureScaleBias);
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
        return submissions;
    }

    void StaticTrackScene::DrawSky(const Matrix& view, const Matrix& projection)
    {
        auto& parameters = skyEffect->getParametersProperty();
        parameters["view"]->SetValue(view);
        parameters["projection"]->SetValue(projection);
        parameters["ambientColor"]->SetValue(Color(232, 232, 232).ToVector4());
        parameters["diffuseTexture"]->SetValue(&*skyCubeTexture);
        EffectTechnique* technique =
            skyEffect->getTechniquesProperty()["SkyCubeMap"];
        if (!technique)
            throw std::runtime_error("Missing Racing SkyCubeMap technique");
        skyEffect->setCurrentTechniqueProperty(technique);
        device.setDepthStencilStateProperty(DepthStencilState::None);
        device.setRasterizerStateProperty(RasterizerState::CullNone);
        device.setBlendStateProperty(BlendState::Opaque);
        for (ModelMesh* mesh : skyCubeModel->getMeshesProperty())
        {
            for (ModelMeshPart* part : mesh->getMeshPartsProperty())
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
            }
        }
    }

    void StaticTrackScene::AddBrakeTrack(
        const GameLogic::CarPhysics& car)
    {
        Vector3 position = car.getCarPositionProperty() +
                           car.getCarDirectionProperty() * 1.25f;
        constexpr int maximumVertices = 6 * 140;
        if (Vector3::DistanceSquared(
                position, lastAddedBrakeTrackPosition) < 0.024f ||
            static_cast<int>(brakeTrackVertices.size()) > maximumVertices)
            return;

        lastAddedBrakeTrackPosition = position;
        constexpr float width = 2.4f;
        constexpr float length = 4.5f;
        const float maximumDistance =
            std::sqrt(width * width + length * length) / 2.0f - 0.35f;
        for (const TangentVertex& vertex : brakeTrackVertices)
        {
            if (Vector3::DistanceSquared(vertex.pos, position) <
                maximumDistance * maximumDistance)
                return;
        }

        const Vector3 direction = car.getCarDirectionProperty();
        const Vector3 right = car.getCarRightProperty();
        const Vector3 up = car.getCarUpVectorProperty();
        position += Vector3::Normalize(up) * 0.2f;
        brakeTrackVertices.insert(brakeTrackVertices.end(), {
            {position - right * width / 2.0f - direction * length / 2.0f,
             Vector2(0.0f, 0.0f), up, right},
            {position - right * width / 2.0f + direction * length / 2.0f,
             Vector2(0.0f, 5.0f), up, right},
            {position + right * width / 2.0f + direction * length / 2.0f,
             Vector2(1.0f, 5.0f), up, right},
            {position - right * width / 2.0f - direction * length / 2.0f,
             Vector2(0.0f, 0.0f), up, right},
            {position + right * width / 2.0f + direction * length / 2.0f,
             Vector2(1.0f, 5.0f), up, right},
            {position + right * width / 2.0f - direction * length / 2.0f,
             Vector2(1.0f, 0.0f), up, right},
        });
    }

    void StaticTrackScene::DrawBrakeTracks(
        const Matrix& view, const Matrix& projection)
    {
        lastBrakeTrackPrimitiveCount = 0;
        if (brakeTrackVertices.empty()) return;

        SetCommonParameters(*brakeTrackEffect, view, projection,
                            Matrix::getIdentityProperty());
        auto& parameters = brakeTrackEffect->getParametersProperty();
        if (EffectParameter* parameter = parameters["ambientColor"])
            parameter->SetValue(Color(40, 40, 40).ToVector4());
        if (EffectParameter* parameter = parameters["diffuseColor"])
            parameter->SetValue(Color(210, 210, 210).ToVector4());
        if (EffectParameter* parameter = parameters["specularColor"])
            parameter->SetValue(Color(255, 255, 255).ToVector4());
        if (EffectParameter* parameter = parameters["specularPower"])
            parameter->SetValue(24.0f);
        if (EffectParameter* parameter = parameters["diffuseTexture"])
            parameter->SetValue(&*brakeTrackDiffuse);
        EffectTechnique* technique =
            brakeTrackEffect->getTechniquesProperty()["Diffuse20"];
        if (!technique)
            throw std::runtime_error(
                "Missing Racing brake-track Diffuse20 technique");
        brakeTrackEffect->setCurrentTechniqueProperty(technique);
        device.setBlendStateProperty(BlendState::AlphaBlend);
        for (EffectPass& pass : technique->getPassesProperty())
        {
            pass.Apply();
            device.DrawUserPrimitives(
                PrimitiveType::TriangleList, brakeTrackVertices.data(), 0,
                static_cast<int>(brakeTrackVertices.size() / 3),
                TangentVertex::getVertexDeclarationStatic());
        }
        lastBrakeTrackPrimitiveCount =
            static_cast<int>(brakeTrackVertices.size() / 3);
    }

    void StaticTrackScene::DrawGeometry(
        const Matrix& view, const Matrix& projection,
        const bool includeCityPlane)
    {
        DrawSky(view, projection);
        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);

        SetCommonParameters(*landscapeEffect, view, projection,
                            Matrix::getIdentityProperty());
        SetMaterialParameters(*landscapeEffect, *landscapeDiffuse,
                              *landscapeNormal, Color(88, 88, 88),
                              Color(234, 234, 234), Color(33, 33, 33));
        if (EffectParameter* detail =
                landscapeEffect->getParametersProperty()["detailTexture"])
            detail->SetValue(&*landscapeDetail);
        DrawMesh(landscapeMesh, *landscapeEffect, "DiffuseWithDetail20");

        lastCityPlaneSubmissionCount = 0;
        if (includeCityPlane && cityPlaneAnchor && cityPlaneMesh.vertexBuffer)
        {
            SetCommonParameters(
                *normalEffect, view, projection,
                Matrix::CreateTranslation(*cityPlaneAnchor));
            SetMaterialParameters(*normalEffect, *cityDiffuse, *cityNormal,
                                  Color(32, 32, 32), Color(200, 200, 200),
                                  Color(128, 128, 128));
            DrawMesh(cityPlaneMesh, *normalEffect, "DiffuseSpecular20");
            lastCityPlaneSubmissionCount = 1;
        }

        SetCommonParameters(*normalEffect, view, projection,
                            Matrix::getIdentityProperty());
        SetMaterialParameters(*normalEffect, *roadDiffuse, *roadNormal,
                              Color(40, 40, 40), Color(210, 210, 210),
                              Color(255, 255, 255));
        DrawMesh(roadMesh, *normalEffect,
                 highDetail ? "SpecularRoad20" : "Specular20");

        SetMaterialParameters(*normalEffect, *roadBackDiffuse, *roadBackNormal,
                              Color(40, 40, 40), Color(210, 210, 210),
                              Color(255, 255, 255));
        DrawMesh(roadBackMesh, *normalEffect,
                 highDetail ? "SpecularRoad20" : "Specular20");

        device.setRasterizerStateProperty(RasterizerState::CullNone);
        SetMaterialParameters(*normalEffect, *tunnelDiffuse, *tunnelNormal,
                              Color(182, 182, 182), Color(80, 80, 80),
                              Color(64, 64, 64));
        DrawMesh(tunnelMesh, *normalEffect, "Diffuse20");
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);

        SetMaterialParameters(*normalEffect, *guardDiffuse, *guardNormal,
                              Color(72, 72, 72), Color(182, 182, 182),
                              Color(225, 225, 225));
        DrawMesh(leftGuardMesh, *normalEffect, "Specular20");
        DrawMesh(rightGuardMesh, *normalEffect, "Specular20");

        SetMaterialParameters(*normalEffect, *columnDiffuse, *columnNormal,
                              Color(40, 40, 40), Color(210, 210, 210),
                              Color(255, 255, 255));
        DrawMesh(columnsMesh, *normalEffect, "Specular20");
    }

    void StaticTrackScene::DrawStaticGeometry(
        const Matrix& view, const Matrix& projection)
    {
        DrawGeometry(view, projection, false);
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
    }

    void StaticTrackScene::Draw(const Matrix& view, const Matrix& projection,
                                const float totalTimeSeconds)
    {
        DrawGeometry(view, projection, true);
        lastLandscapeModelPartCount = landscapeObjects->Draw(
            view, projection, totalTimeSeconds);
        DrawBrakeTracks(view, projection);
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
    }

    int StaticTrackScene::getLandscapeModelCountProperty() const
    {
        return landscapeObjects->getLoadedModelCountProperty();
    }

    int StaticTrackScene::getLandscapeObjectCountProperty() const
    {
        return landscapeObjects->getObjectCountProperty();
    }

    int StaticTrackScene::getLastLandscapeModelPartCountProperty() const
    {
        return lastLandscapeModelPartCount;
    }

    int StaticTrackScene::getLastCityPlaneSubmissionCountProperty() const
    {
        return lastCityPlaneSubmissionCount;
    }

    int StaticTrackScene::getBrakeTrackVertexCountProperty() const
    {
        return static_cast<int>(brakeTrackVertices.size());
    }

    int StaticTrackScene::getLastBrakeTrackPrimitiveCountProperty() const
    {
        return lastBrakeTrackPrimitiveCount;
    }

    void StaticTrackScene::ReplaceStartLightObject(const int number)
    {
        landscapeObjects->ReplaceStartLightObject(number);
    }

    void StaticTrackScene::setHighDetailProperty(const bool value)
    {
        highDetail = value;
        landscapeObjects->setHighDetailProperty(value);
    }

    const Tracks::TrackLine& StaticTrackScene::getTrackLineProperty() const
    {
        return track;
    }

    const Tracks::Track& StaticTrackScene::getTrackProperty() const
    {
        return track;
    }

    const std::vector<Matrix>&
    StaticTrackScene::getLeftHolderMatricesProperty() const
    {
        return leftGuard.getHolderMatricesProperty();
    }

    const std::vector<Matrix>&
    StaticTrackScene::getRightHolderMatricesProperty() const
    {
        return rightGuard.getHolderMatricesProperty();
    }

    const std::vector<Vector3>&
    StaticTrackScene::getColumnSegmentPositionsProperty() const
    {
        return columns.getSegmentPositionsProperty();
    }
}
