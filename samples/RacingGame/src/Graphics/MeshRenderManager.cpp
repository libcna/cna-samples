// SPDX-License-Identifier: MS-PL

#include "Graphics/MeshRenderManager.hpp"

#include <stdexcept>

#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"

namespace RacingGame::Graphics
{
    using Microsoft::Xna::Framework::Matrix;
    using namespace Microsoft::Xna::Framework::Graphics;

    MeshRenderManager::RenderableMesh::RenderableMesh(
        ModelMeshPart& setPart, Material setMaterial,
        std::string setTechniqueName)
        : part(&setPart), material(std::move(setMaterial)),
          techniqueName(std::move(setTechniqueName))
    {
    }

    void MeshRenderManager::RenderableMesh::AddRenderMatrix(Matrix worldMatrix)
    {
        renderMatrices.push_back(worldMatrix);
    }

    MeshRenderManager::RenderableMesh* MeshRenderManager::Add(
        ModelMeshPart& part, Effect& sourceEffect,
        const std::string& techniqueName)
    {
        auto mesh = std::unique_ptr<RenderableMesh>(new RenderableMesh(
            part, Material(sourceEffect), techniqueName));
        RenderableMesh* result = mesh.get();
        meshes.push_back(std::move(mesh));

        TechniqueGroup* techniqueGroup = nullptr;
        for (TechniqueGroup& candidate : techniques)
        {
            if (candidate.name == techniqueName)
            {
                techniqueGroup = &candidate;
                break;
            }
        }
        if (!techniqueGroup)
        {
            techniques.emplace_back(techniqueName);
            techniqueGroup = &techniques.back();
        }

        MaterialGroup* materialGroup = nullptr;
        for (MaterialGroup& candidate : techniqueGroup->materials)
        {
            if (candidate.material.Equals(result->material))
            {
                materialGroup = &candidate;
                break;
            }
        }
        if (!materialGroup)
        {
            techniqueGroup->materials.emplace_back(result->material);
            materialGroup = &techniqueGroup->materials.back();
        }
        materialGroup->meshes.push_back(result);
        return result;
    }

    int MeshRenderManager::Render(
        GraphicsDevice& device, Effect& effect, const Matrix& view,
        const Matrix& projection,
        const Microsoft::Xna::Framework::Vector3& lightDirection,
        TextureCube* reflectionTexture)
    {
        device.setDepthStencilStateProperty(DepthStencilState::Default);
        const Matrix viewProjection = view * projection;
        auto& parameters = effect.getParametersProperty();
        if (EffectParameter* parameter = parameters["viewProj"])
            parameter->SetValue(viewProjection);
        if (EffectParameter* parameter = parameters["viewInverse"])
            parameter->SetValue(Matrix::Invert(view));
        if (EffectParameter* parameter = parameters["lightDir"])
            parameter->SetValue(lightDirection);
        if (EffectParameter* parameter = parameters["reflectionCubeTexture"])
            parameter->SetValue(reflectionTexture);

        VertexBuffer* lastVertexBuffer = nullptr;
        IndexBuffer* lastIndexBuffer = nullptr;
        int submissions = 0;
        for (TechniqueGroup& techniqueGroup : techniques)
        {
            EffectTechnique* technique =
                effect.getTechniquesProperty()[techniqueGroup.name];
            if (!technique)
                throw std::runtime_error(
                    "Missing authentic Racing model technique: " +
                    techniqueGroup.name);
            effect.setCurrentTechniqueProperty(technique);

            for (MaterialGroup& materialGroup : techniqueGroup.materials)
            {
                const Material& material = materialGroup.material;
                if (EffectParameter* parameter = parameters["diffuseTexture"])
                    parameter->SetValue(material.getDiffuseTextureProperty());
                if (EffectParameter* parameter = parameters["normalTexture"])
                    parameter->SetValue(material.getNormalTextureProperty());
                if (EffectParameter* parameter = parameters["diffuseColor"])
                    parameter->SetValue(material.getDiffuseColorProperty());
                if (EffectParameter* parameter = parameters["ambientColor"])
                    parameter->SetValue(material.getAmbientColorProperty());
                if (EffectParameter* parameter = parameters["specularColor"])
                    parameter->SetValue(material.getSpecularColorProperty());
                if (EffectParameter* parameter = parameters["specularPower"])
                    parameter->SetValue(material.getSpecularPowerProperty());
                if (EffectParameter* parameter = parameters["UseAlpha"])
                    parameter->SetValue(material.getHasAlphaProperty());
                if (material.getHasAlphaProperty())
                    device.setRasterizerStateProperty(RasterizerState::CullNone);
                for (RenderableMesh* mesh : materialGroup.meshes)
                {
                    if (mesh->renderMatrices.empty()) continue;
                    VertexBuffer* vertexBuffer =
                        mesh->part->getVertexBufferProperty();
                    IndexBuffer* indexBuffer =
                        mesh->part->getIndexBufferProperty();
                    if (lastVertexBuffer != vertexBuffer ||
                        lastIndexBuffer != indexBuffer)
                    {
                        device.SetVertexBuffer(vertexBuffer);
                        device.setIndicesProperty(indexBuffer);
                        lastVertexBuffer = vertexBuffer;
                        lastIndexBuffer = indexBuffer;
                    }
                    for (const Matrix& world : mesh->renderMatrices)
                    {
                        if (EffectParameter* parameter = parameters["world"])
                            parameter->SetValue(world);
                        for (EffectPass& pass : technique->getPassesProperty())
                        {
                            pass.Apply();
                            device.DrawIndexedPrimitives(
                                PrimitiveType::TriangleList,
                                mesh->part->getVertexOffsetProperty(), 0,
                                mesh->part->getNumVerticesProperty(),
                                mesh->part->getStartIndexProperty(),
                                mesh->part->getPrimitiveCountProperty());
                            ++submissions;
                        }
                    }
                    mesh->renderMatrices.clear();
                }
                if (material.getHasAlphaProperty())
                    device.setRasterizerStateProperty(
                        RasterizerState::CullCounterClockwise);
            }
        }
        device.SetVertexBuffer(nullptr);
        device.setIndicesProperty(nullptr);
        return submissions;
    }

    int MeshRenderManager::getRegisteredMeshCountProperty() const
    {
        return static_cast<int>(meshes.size());
    }
}
