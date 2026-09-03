// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionColor.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"

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
    class ResolutionMapper;

    /** @brief Collects and renders the original screen-space colored lines. */
    class LineManager2D
    {
    public:
        /**
         * @brief Loads the authentic line effect for one graphics device.
         * @param device Device receiving line primitives.
         * @param content Content manager used to load LineRendering.xnb.
         * @param mapper Current Racing resolution mapper.
         */
        LineManager2D(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Content::ContentManager& content,
            ResolutionMapper& mapper);

        /**
         * @brief Adds one line for the current frame.
         * @param startPoint First screen-space endpoint.
         * @param endPoint Second screen-space endpoint.
         * @param color Per-vertex line color.
         */
        void AddLine(Microsoft::Xna::Framework::Point startPoint,
                     Microsoft::Xna::Framework::Point endPoint,
                     Microsoft::Xna::Framework::Color color);
        /**
         * @brief Adds a line plus its one-pixel black shadow.
         * @param startPoint First screen-space endpoint.
         * @param endPoint Second screen-space endpoint.
         * @param color Foreground line color.
         */
        void AddLineWithShadow(Microsoft::Xna::Framework::Point startPoint,
                               Microsoft::Xna::Framework::Point endPoint,
                               Microsoft::Xna::Framework::Color color);
        /** @brief Renders collected lines and starts a fresh frame. */
        void Render();
        /** @brief Gets the number of line primitives submitted most recently. */
        [[nodiscard]] int getLastPrimitiveCountProperty() const;

    private:
        static constexpr int MaxNumOfLines = 64;

        struct Line
        {
            Microsoft::Xna::Framework::Point startPoint;
            Microsoft::Xna::Framework::Point endPoint;
            Microsoft::Xna::Framework::Color color;

            [[nodiscard]] bool operator==(const Line&) const = default;
        };

        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        ResolutionMapper& mapper;
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
