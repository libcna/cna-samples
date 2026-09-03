// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "Graphics/MeshRenderManager.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Tracks/TrackCombiModels.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class GraphicsDevice;
    class TextureCube;
}

namespace RacingGame::Graphics
{
    class Model;
}

namespace RacingGame::Landscapes
{
    class Landscape;
}

namespace RacingGame::Tracks
{
    class Track;
}

namespace RacingGame::Rendering
{
    /** @brief Owns, places and batches the original Racing landscape models. */
    class LandscapeObjectRenderer
    {
    public:
        /**
         * @brief Loads the complete authentic model catalog and resolves track objects.
         * @param device Graphics device receiving model batches.
         * @param content Content manager rooted at the authentic XNA build.
         * @param landscape Landscape height and authored-object source.
         * @param track Generated track used for start-line placement.
         * @param leftHolders Generated left guard-rail holder transforms.
         * @param rightHolders Generated right guard-rail holder transforms.
         * @param columnSegments Generated road-column foot positions.
         */
        LandscapeObjectRenderer(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Content::ContentManager& content,
            const Landscapes::Landscape& landscape,
            const Tracks::Track& track,
            const std::vector<Microsoft::Xna::Framework::Matrix>& leftHolders,
            const std::vector<Microsoft::Xna::Framework::Matrix>& rightHolders,
            const std::vector<Microsoft::Xna::Framework::Vector3>& columnSegments);
        /** @brief Destroys owned model wrappers after their complete type is available. */
        ~LandscapeObjectRenderer();

        /**
         * @brief Queues and draws all currently visible landscape objects.
         * @param view Current camera view matrix.
         * @param projection Current camera projection matrix.
         * @param totalTimeSeconds Total game time for animated model parts.
         * @return Number of indexed model-part submissions.
         */
        int Draw(const Microsoft::Xna::Framework::Matrix& view,
                 const Microsoft::Xna::Framework::Matrix& projection,
                 float totalTimeSeconds);

        /** @brief Gets the number of authentic catalog models loaded. */
        [[nodiscard]] int getLoadedModelCountProperty() const;
        /** @brief Gets the number of final objects after combinations and collision filtering. */
        [[nodiscard]] int getObjectCountProperty() const;
        /** @brief Gets the number of final objects marked near the track. */
        [[nodiscard]] int getNearTrackObjectCountProperty() const;
        /** @brief Gets the number of registered processed model parts. */
        [[nodiscard]] int getRegisteredMeshCountProperty() const;

        /**
         * @brief Replaces the remembered start light with the requested original model.
         * @param number Light state 0=red, 1=yellow, 2=green; invalid values select red.
         */
        void ReplaceStartLightObject(int number);

    private:
        struct LandscapeObject
        {
            Graphics::Model* model = nullptr;
            Microsoft::Xna::Framework::Matrix matrix;
            bool isNearTrack = false;
        };

        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        const Landscapes::Landscape& landscape;
        Graphics::MeshRenderManager meshManager;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> normalEffect;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::TextureCube>
            reflectionTexture;
        std::vector<std::unique_ptr<Graphics::Model>> models;
        std::unordered_map<std::string, Graphics::Model*> modelByName;
        std::vector<Tracks::TrackCombiModels> combinations;
        std::vector<LandscapeObject> objects;
        std::optional<std::size_t> startLightObjectIndex;

        void AddObject(const std::string& modelName,
                       Microsoft::Xna::Framework::Matrix matrix,
                       bool isNearTrack);
        void AddObject(const std::string& modelName, float rotation,
                       Microsoft::Xna::Framework::Vector3 trackPosition,
                       Microsoft::Xna::Framework::Vector3 trackRight,
                       float distance);
        void GenerateObjectsForTrack(const Tracks::Track& track);
        [[nodiscard]] float GetObjectSize(const std::string& modelName) const;
        [[nodiscard]] static std::string FixModelName(std::string modelName);
        [[nodiscard]] static std::string ToLower(std::string value);
    };
}
