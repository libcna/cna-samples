// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Graphics/Material.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class GraphicsDevice;
    class ModelMeshPart;
    class TextureCube;
}

namespace RacingGame::Graphics
{
    /** @brief Batches Racing model parts by original effect technique and material. */
    class MeshRenderManager
    {
    public:
        /** @brief One immutable model part plus its per-frame world transforms. */
        class RenderableMesh
        {
        public:
            /** @brief Adds one visible world transform to this frame's batch. */
            void AddRenderMatrix(
                Microsoft::Xna::Framework::Matrix worldMatrix);

        private:
            RenderableMesh(
                Microsoft::Xna::Framework::Graphics::ModelMeshPart& part,
                Material setMaterial, std::string setTechniqueName);

            Microsoft::Xna::Framework::Graphics::ModelMeshPart* part;
            Material material;
            std::string techniqueName;
            std::vector<Microsoft::Xna::Framework::Matrix> renderMatrices;

            friend class MeshRenderManager;
        };

        /**
         * @brief Registers one processed model part in its permanent batch.
         * @param part Processed XNA model part.
         * @param sourceEffect Effect carrying the part's authored material.
         * @param techniqueName Technique selected by the original mesh-name rule.
         * @return Stable renderable owned by this manager.
         */
        [[nodiscard]] RenderableMesh* Add(
            Microsoft::Xna::Framework::Graphics::ModelMeshPart& part,
            Microsoft::Xna::Framework::Graphics::Effect& sourceEffect,
            const std::string& techniqueName);

        /**
         * @brief Draws and clears every queued matrix using the shared Racing effect.
         * @param device Graphics device receiving the batches.
         * @param effect Authentic shared NormalMapping effect.
         * @param view Current camera view.
         * @param projection Current camera projection.
         * @param lightDirection Original world light direction.
         * @param reflectionTexture Original sky cube used by reflective techniques.
         * @return Number of indexed draw submissions.
         */
        int Render(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection,
            const Microsoft::Xna::Framework::Vector3& lightDirection,
            Microsoft::Xna::Framework::Graphics::TextureCube* reflectionTexture);

        /** @brief Gets the number of registered immutable model parts. */
        [[nodiscard]] int getRegisteredMeshCountProperty() const;

    private:
        struct MaterialGroup
        {
            explicit MaterialGroup(Material setMaterial)
                : material(std::move(setMaterial)) {}
            Material material;
            std::vector<RenderableMesh*> meshes;
        };

        struct TechniqueGroup
        {
            explicit TechniqueGroup(std::string setName)
                : name(std::move(setName)) {}
            std::string name;
            std::vector<MaterialGroup> materials;
        };

        std::vector<std::unique_ptr<RenderableMesh>> meshes;
        std::vector<TechniqueGroup> techniques;
    };
}
