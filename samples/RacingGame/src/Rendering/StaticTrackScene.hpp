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

namespace RacingGame::GameLogic
{
    class CarPhysics;
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
         * @brief Draws the complete terrain, track, populated world and decals.
         *
         * @param view Camera view matrix.
         * @param projection Camera projection matrix.
         * @param totalTimeSeconds Total game time used by animated world models.
         */
        void Draw(const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection,
                  float totalTimeSeconds = 0.0f);

        /**
         * @brief Draws the deterministic terrain and generated track geometry.
         *
         * This excludes separately populated landscape models, the city plane and
         * brake-track decals.
         *
         * @param view Camera view matrix.
         * @param projection Camera projection matrix.
         */
        void DrawStaticGeometry(
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection);

        /**
         * @brief Draws the original track and near-track casters into the shadow map.
         * @param effect Authentic ShadowMap effect using GenerateShadowMap20.
         * @param lightViewProjection Current light view-projection matrix.
         * @param shadowLightPosition Current virtual light position.
         * @param shadowDistance Original shadow distance.
         * @param totalTimeSeconds Total game time used by windmill animation.
         * @return Number of submitted geometry batches.
         */
        int GenerateShadows(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            const Microsoft::Xna::Framework::Matrix& lightViewProjection,
            Microsoft::Xna::Framework::Vector3 shadowLightPosition,
            float shadowDistance, float totalTimeSeconds);

        /**
         * @brief Draws the original terrain, objects and track into the receiver map.
         * @param effect Authentic ShadowMap effect using UseShadowMap20.
         * @param viewProjection Current camera view-projection matrix.
         * @param lightViewProjection Current light view-projection matrix.
         * @param textureScaleBias Light projection to shadow-texture transform.
         * @param shadowLightPosition Current virtual light position.
         * @param shadowDistance Original shadow distance.
         * @param totalTimeSeconds Total game time used by windmill animation.
         * @return Number of submitted geometry batches.
         */
        int UseShadows(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            const Microsoft::Xna::Framework::Matrix& viewProjection,
            const Microsoft::Xna::Framework::Matrix& lightViewProjection,
            const Microsoft::Xna::Framework::Matrix& textureScaleBias,
            Microsoft::Xna::Framework::Vector3 shadowLightPosition,
            float shadowDistance, float totalTimeSeconds);

        /** @brief Gets the number of authentic landscape models loaded. */
        [[nodiscard]] int getLandscapeModelCountProperty() const;
        /** @brief Gets the number of resolved landscape model instances. */
        [[nodiscard]] int getLandscapeObjectCountProperty() const;
        /** @brief Gets the number of model-part submissions in the latest draw. */
        [[nodiscard]] int getLastLandscapeModelPartCountProperty() const;
        /** @brief Gets whether the original city-ground plane was submitted. */
        [[nodiscard]] int getLastCityPlaneSubmissionCountProperty() const;
        /**
         * @brief Adds one source-faithful tire-mark quad for a major brake event.
         * @param car Current car pose used by the original decal calculation.
         */
        void AddBrakeTrack(const GameLogic::CarPhysics& car);
        /** @brief Gets the number of retained brake-track vertices. */
        [[nodiscard]] int getBrakeTrackVertexCountProperty() const;
        /** @brief Gets the brake-track triangle count submitted most recently. */
        [[nodiscard]] int getLastBrakeTrackPrimitiveCountProperty() const;
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
        int lastCityPlaneSubmissionCount = 0;
        int lastBrakeTrackPrimitiveCount = 0;
        std::optional<Microsoft::Xna::Framework::Vector3> cityPlaneAnchor;
        std::vector<Graphics::TangentVertex> brakeTrackVertices;
        Microsoft::Xna::Framework::Vector3 lastAddedBrakeTrackPosition =
            Microsoft::Xna::Framework::Vector3(-1000.0f, -1000.0f, -1000.0f);

        GpuMesh landscapeMesh;
        GpuMesh cityPlaneMesh;
        GpuMesh roadMesh;
        GpuMesh roadBackMesh;
        GpuMesh tunnelMesh;
        GpuMesh leftGuardMesh;
        GpuMesh rightGuardMesh;
        GpuMesh columnsMesh;

        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> normalEffect;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect>
            brakeTrackEffect;
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
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> cityDiffuse;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> cityNormal;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>
            brakeTrackDiffuse;
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
        int DrawShadowMesh(
            GpuMesh& mesh,
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            const Microsoft::Xna::Framework::Matrix& viewProjection,
            const Microsoft::Xna::Framework::Matrix& lightViewProjection,
            const Microsoft::Xna::Framework::Matrix* textureScaleBias);
        void DrawSky(const Microsoft::Xna::Framework::Matrix& view,
                     const Microsoft::Xna::Framework::Matrix& projection);
        void DrawBrakeTracks(
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection);
        void DrawGeometry(
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection,
            bool includeCityPlane);
        void SetCommonParameters(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection,
            const Microsoft::Xna::Framework::Matrix& world);
        static void SetMaterialParameters(
            Microsoft::Xna::Framework::Graphics::Effect& effect,
            Microsoft::Xna::Framework::Graphics::Texture2D& diffuse,
            Microsoft::Xna::Framework::Graphics::Texture2D& normal,
            const Microsoft::Xna::Framework::Color& ambientColor,
            const Microsoft::Xna::Framework::Color& diffuseColor,
            const Microsoft::Xna::Framework::Color& specularColor);
    };
}
