// SPDX-License-Identifier: MS-PL

#pragma once

#include <string>
#include <vector>

#include "Graphics/MeshRenderManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace RacingGame::Graphics
{
    /** @brief Loads and queues one original Racing static model. */
    class Model
    {
    public:
        /**
         * @brief Loads an authentic XNA model and registers all of its mesh parts.
         * @param setModelName Original model content name without its directory.
         * @param content Content manager rooted at the authentic XNA build.
         * @param meshManager Shared model batching owner.
         */
        Model(
            const std::string& setModelName,
            Microsoft::Xna::Framework::Content::ContentManager& content,
            MeshRenderManager& meshManager);

        /** @brief Gets the original model content name. */
        [[nodiscard]] const std::string& getNameProperty() const;
        /** @brief Gets the original first-mesh radius used for object spacing. */
        [[nodiscard]] float getSizeProperty() const;
        /** @brief Gets the number of registered mesh parts. */
        [[nodiscard]] int getNumOfMeshPartsProperty() const;

        /**
         * @brief Culls and queues one model instance exactly like the original model wrapper.
         * @param renderMatrix Authored landscape object transform.
         * @param cameraPosition Current camera position.
         * @param cameraRotation Current camera forward direction.
         * @param totalTimeSeconds Total game time used by windmill animation.
         */
        void Render(
            Microsoft::Xna::Framework::Matrix renderMatrix,
            Microsoft::Xna::Framework::Vector3 cameraPosition,
            Microsoft::Xna::Framework::Vector3 cameraRotation,
            float totalTimeSeconds);

    private:
        struct MeshEntry
        {
            Microsoft::Xna::Framework::Graphics::ModelMesh* mesh = nullptr;
            std::vector<MeshRenderManager::RenderableMesh*> renderables;
        };

        static constexpr int MaxViewDistance = 200;
        std::string name;
        Microsoft::Xna::Framework::Graphics::Model xnaModel;
        std::vector<Microsoft::Xna::Framework::Matrix> transforms;
        std::vector<MeshEntry> meshEntries;
        float realScaling = 1.0f;
        float scaling = 1.0f;
        Microsoft::Xna::Framework::Graphics::ModelMesh* animatedMesh = nullptr;
        int meshPartCount = 0;

        [[nodiscard]] static std::string ToLower(std::string value);
        [[nodiscard]] static std::string SelectTechniqueName(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            const std::string& meshName, int partIndex);
    };
}
