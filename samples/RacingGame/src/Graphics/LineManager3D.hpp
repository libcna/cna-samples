// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class GraphicsDevice;
}

namespace RacingGame::Graphics
{
    /** @brief Collects and renders the original world-space colored lines. */
    class LineManager3D
    {
    public:
        /**
         * @brief Loads the authentic line effect for one graphics device.
         * @param device Device receiving line primitives.
         * @param content Content manager used to load LineRendering.xnb.
         */
        LineManager3D(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Content::ContentManager& content);

        /**
         * @brief Adds one line with independent endpoint colors.
         * @param startPoint First world-space endpoint.
         * @param startColor Color at the first endpoint.
         * @param endPoint Second world-space endpoint.
         * @param endColor Color at the second endpoint.
         */
        void AddLine(Microsoft::Xna::Framework::Vector3 startPoint,
                     Microsoft::Xna::Framework::Color startColor,
                     Microsoft::Xna::Framework::Vector3 endPoint,
                     Microsoft::Xna::Framework::Color endColor);

        /**
         * @brief Adds one line using the same color at both endpoints.
         * @param startPoint First world-space endpoint.
         * @param endPoint Second world-space endpoint.
         * @param color Per-vertex line color.
         */
        void AddLine(Microsoft::Xna::Framework::Vector3 startPoint,
                     Microsoft::Xna::Framework::Vector3 endPoint,
                     Microsoft::Xna::Framework::Color color);

        /**
         * @brief Renders collected lines through the original 3D technique.
         * @param world Current world matrix.
         * @param view Current view matrix.
         * @param projection Current projection matrix.
         */
        void Render(const Microsoft::Xna::Framework::Matrix& world,
                    const Microsoft::Xna::Framework::Matrix& view,
                    const Microsoft::Xna::Framework::Matrix& projection);

        /** @brief Gets the number of line primitives submitted most recently. */
        [[nodiscard]] int getLastPrimitiveCountProperty() const;

    private:
        static constexpr int MaxNumOfLines = 4096;

        struct Line
        {
            Microsoft::Xna::Framework::Vector3 startPoint;
            Microsoft::Xna::Framework::Color startColor;
            Microsoft::Xna::Framework::Vector3 endPoint;
            Microsoft::Xna::Framework::Color endColor;

            [[nodiscard]] bool operator==(const Line&) const = default;
        };

        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> effect;
        std::vector<Line> lines;
        std::array<Microsoft::Xna::Framework::Graphics::VertexPositionColor,
                   MaxNumOfLines * 2> lineVertices{};
        int numOfLines = 0;
        int numOfPrimitives = 0;
        int lastPrimitiveCount = 0;
        bool buildVertexBuffer = false;

        void UpdateVertexBuffer();
    };
}
