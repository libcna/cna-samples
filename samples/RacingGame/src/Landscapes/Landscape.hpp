// SPDX-License-Identifier: MS-PL

#pragma once

#include <cstdint>
#include <vector>

#include "Graphics/TangentVertex.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

namespace RacingGame::Landscapes
{
    using SharpRuntime::String;
    using SharpRuntime::bytecs;

    class Landscape
    {
    public:
        struct LandscapeObjectRecord
        {
            String modelName;
            Microsoft::Xna::Framework::Matrix matrix;
            bool isNearTrackForShadowGeneration = false;
        };

        explicit Landscape(
            const String& heightPath = "Content\\LandscapeHeights.data");

        void KillAllLoadedObjects();
        void AddObjectToRender(
            const String& modelName,
            Microsoft::Xna::Framework::Matrix renderMatrix,
            bool isNearTrackForShadowGeneration);

        [[nodiscard]] float GetMapHeight(int x, int y) const;
        [[nodiscard]] float GetMapHeight(float x, float y) const;

        [[nodiscard]] const std::vector<Graphics::TangentVertex>&
        getVerticesProperty() const;
        [[nodiscard]] const std::vector<std::uint32_t>& getIndicesProperty() const;
        [[nodiscard]] const std::vector<LandscapeObjectRecord>&
        getLandscapeObjectRecordsProperty() const;

    private:
        static constexpr int GridWidth = 257;
        static constexpr int GridHeight = 257;
        static constexpr float MapWidthFactor = 10.0f;
        static constexpr float MapHeightFactor = 10.0f;
        static constexpr float MapZScale = 300.0f;

        std::vector<Graphics::TangentVertex> vertices;
        std::vector<std::uint32_t> indices;
        std::vector<float> mapHeights;
        std::vector<LandscapeObjectRecord> landscapeObjects;

        [[nodiscard]] static int ModulateValueInRange(float value, int maximum);
        [[nodiscard]] static Microsoft::Xna::Framework::Vector3 CalcLandscapePos(
            int x, int y, const std::vector<bytecs>& heights);
        [[nodiscard]] static std::size_t HeightIndex(int x, int y);
    };
}
