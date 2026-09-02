// SPDX-License-Identifier: MS-PL

#include "Rendering/StaticTrackScene.hpp"

#include <stdexcept>

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
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace RacingGame::Rendering
{
    using Graphics::TangentVertex;
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Vector4;
    using namespace Microsoft::Xna::Framework::Graphics;

    StaticTrackScene::StaticTrackScene(
        GraphicsDevice& setDevice,
        Microsoft::Xna::Framework::Content::ContentManager& content,
        const SharpRuntime::String& trackName)
        : device(setDevice),
          landscape(),
          track(trackName, landscape),
          trackGeometry(track),
          leftGuard(track, Tracks::GuardRailGeometry::Mode::Left),
          rightGuard(track, Tracks::GuardRailGeometry::Mode::Right),
          columns(track, landscape)
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
        landscapeEffect = content.Load<std::shared_ptr<Effect>>(
            "Shaders/LandscapeNormalMapping");
        skyEffect = content.Load<std::shared_ptr<Effect>>(
            "Shaders/PreScreenSkyCubeMapping");
        skyCubeModel.emplace(content.Load<Model>("Models/Cube"));
        skyCubeTexture.emplace(content.Load<TextureCube>("Textures/SkyCubeMap"));
        landscapeDiffuse.emplace(content.Load<Texture2D>("Textures/Landscape"));
        landscapeNormal.emplace(content.Load<Texture2D>("Textures/LandscapeNormal"));
        landscapeDetail.emplace(content.Load<Texture2D>("Textures/LandscapeDetail"));
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
        if (!normalEffect || !landscapeEffect || !skyEffect)
            throw std::runtime_error("Racing static scene effects failed to load");
    }

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
        Effect& effect, const Matrix& view, const Matrix& projection)
    {
        const Matrix world = Matrix::getIdentityProperty();
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

    void StaticTrackScene::Draw(const Matrix& view, const Matrix& projection)
    {
        DrawSky(view, projection);
        device.setBlendStateProperty(BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        device.setRasterizerStateProperty(RasterizerState::CullCounterClockwise);

        SetCommonParameters(*landscapeEffect, view, projection);
        SetMaterialParameters(*landscapeEffect, *landscapeDiffuse,
                              *landscapeNormal, Color(88, 88, 88),
                              Color(234, 234, 234), Color(33, 33, 33));
        if (EffectParameter* detail =
                landscapeEffect->getParametersProperty()["detailTexture"])
            detail->SetValue(&*landscapeDetail);
        DrawMesh(landscapeMesh, *landscapeEffect, "DiffuseWithDetail20");

        SetCommonParameters(*normalEffect, view, projection);
        SetMaterialParameters(*normalEffect, *roadDiffuse, *roadNormal,
                              Color(40, 40, 40), Color(210, 210, 210),
                              Color(255, 255, 255));
        DrawMesh(roadMesh, *normalEffect, "SpecularRoad20");

        SetMaterialParameters(*normalEffect, *roadBackDiffuse, *roadBackNormal,
                              Color(40, 40, 40), Color(210, 210, 210),
                              Color(255, 255, 255));
        DrawMesh(roadBackMesh, *normalEffect, "SpecularRoad20");

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
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
    }

    const Tracks::TrackLine& StaticTrackScene::getTrackLineProperty() const
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
