// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <optional>
#include <vector>

#include "Landscapes/Landscape.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/TextureCube.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Tracks/GuardRailGeometry.hpp"
#include "Tracks/TrackColumnsGeometry.hpp"
#include "Tracks/TrackGeometry.hpp"
#include "Tracks/Track.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace RacingGame::Rendering
{
    class LandscapeObjectRenderer;

    /** @brief Owns the first deterministic GPU-backed Racing track scene. */
    class StaticTrackScene
    {
    public:
        /**
         * @brief Loads authentic materials and uploads generated track geometry.
         *
         * @param device Graphics device that owns the generated buffers.
         * @param content Content manager rooted at the authentic XNA build.
         * @param trackName Original raw track identifier.
         */
        StaticTrackScene(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Content::ContentManager& content,
            const SharpRuntime::String& trackName);
        /** @brief Destroys the complete scene after private renderer types are available. */
        ~StaticTrackScene();

        /**
         * @brief Draws terrain, road, tunnel, rails and columns.
         *
         * @param view Camera view matrix.
         * @param projection Camera projection matrix.
         * @param totalTimeSeconds Total game time used by animated world models.
         */
        void Draw(const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection,
                  float totalTimeSeconds = 0.0f);

        /** @brief Gets the number of authentic landscape models loaded. */
        [[nodiscard]] int getLandscapeModelCountProperty() const;
        /** @brief Gets the number of resolved landscape model instances. */
        [[nodiscard]] int getLandscapeObjectCountProperty() const;
        /** @brief Gets the number of model-part submissions in the latest draw. */
        [[nodiscard]] int getLastLandscapeModelPartCountProperty() const;
        /**
         * @brief Replaces the race start-light model.
         * @param number Original state 0=red, 1=yellow, 2=green.
         */
        void ReplaceStartLightObject(int number);

        /** @brief Gets the generated track line used by this scene. */
        [[nodiscard]] const Tracks::TrackLine& getTrackLineProperty() const;
        /** @brief Gets the complete gameplay track used by this scene. */
        [[nodiscard]] const Tracks::Track& getTrackProperty() const;
        /** @brief Gets left-side guard-rail holder transforms. */
        [[nodiscard]] const std::vector<Microsoft::Xna::Framework::Matrix>&
        getLeftHolderMatricesProperty() const;
        /** @brief Gets right-side guard-rail holder transforms. */
        [[nodiscard]] const std::vector<Microsoft::Xna::Framework::Matrix>&
        getRightHolderMatricesProperty() const;
        /** @brief Gets support-column segment model positions. */
        [[nodiscard]] const std::vector<Microsoft::Xna::Framework::Vector3>&
        getColumnSegmentPositionsProperty() const;

    private:
        struct GpuMesh
        {
            std::unique_ptr<Microsoft::Xna::Framework::Graphics::VertexBuffer>
                vertexBuffer;
            std::unique_ptr<Microsoft::Xna::Framework::Graphics::IndexBuffer>
                indexBuffer;
            int vertexCount = 0;
            int primitiveCount = 0;
        };

        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        Landscapes::Landscape landscape;
        Tracks::Track track;
        Tracks::TrackGeometry trackGeometry;
        Tracks::GuardRailGeometry leftGuard;
        Tracks::GuardRailGeometry rightGuard;
        Tracks::TrackColumnsGeometry columns;
        std::unique_ptr<LandscapeObjectRenderer> landscapeObjects;
        int lastLandscapeModelPartCount = 0;

        GpuMesh landscapeMesh;
        GpuMesh roadMesh;
        GpuMesh roadBackMesh;
        GpuMesh tunnelMesh;
        GpuMesh leftGuardMesh;
        GpuMesh rightGuardMesh;
        GpuMesh columnsMesh;

        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> normalEffect;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> landscapeEffect;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> skyEffect;
        std::optional<Microsoft::Xna::Framework::Graphics::Model> skyCubeModel;
        std::optional<Microsoft::Xna::Framework::Graphics::TextureCube> skyCubeTexture;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
            landscapeDiffuse;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
            landscapeNormal;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
            landscapeDetail;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> roadDiffuse;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> roadNormal;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> roadBackDiffuse;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> roadBackNormal;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> tunnelDiffuse;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> tunnelNormal;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> guardDiffuse;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> guardNormal;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> columnDiffuse;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> columnNormal;

        [[nodiscard]] GpuMesh Upload(
            const std::vector<Graphics::TangentVertex>& vertices,
            const std::vector<std::int32_t>& indices);
        [[nodiscard]] GpuMesh Upload(
            const std::vector<Graphics::TangentVertex>& vertices,
            const std::vector<std::uint32_t>& indices);
        void DrawMesh(GpuMesh& mesh,
                      Microsoft::Xna::Framework::Graphics::Effect& effect,
                      const SharpRuntime::String& techniqueName);
        void DrawSky(const Microsoft::Xna::Framework::Matrix& view,
                     const Microsoft::Xna::Framework::Matrix& projection);
        void SetCommonParameters(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection);
        static void SetMaterialParameters(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            Microsoft::Xna::Framework::Graphics::Texture2D& diffuse,
            Microsoft::Xna::Framework::Graphics::Texture2D& normal,
            const Microsoft::Xna::Framework::Color& ambientColor,
            const Microsoft::Xna::Framework::Color& diffuseColor,
            const Microsoft::Xna::Framework::Color& specularColor);
    };
}
