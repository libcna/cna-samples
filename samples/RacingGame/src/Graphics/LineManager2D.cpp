// SPDX-License-Identifier: MS-PL

#include "Graphics/LineManager2D.hpp"

#include <stdexcept>

#include "Graphics/ResolutionMapper.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace RacingGame::Graphics
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Point;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using namespace Microsoft::Xna::Framework::Graphics;

    LineManager2D::LineManager2D(
        GraphicsDevice& setDevice, ContentManager& content,
        ResolutionMapper& setMapper)
        : device(setDevice), mapper(setMapper),
          effect(content.Load<std::shared_ptr<Effect>>(
              "Shaders/LineRendering"))
    {
        if (!effect || !effect->getTechniquesProperty()["LineRendering2D"])
            throw std::runtime_error(
                "Authentic Racing LineRendering2D effect failed to load");
    }

    void LineManager2D::AddLine(
        const Point startPoint, const Point endPoint, const Color color)
    {
        if (numOfLines >= MaxNumOfLines) return;
        const Line line{startPoint, endPoint, color};
        if (lines.size() > static_cast<std::size_t>(numOfLines))
        {
            if (!(lines[static_cast<std::size_t>(numOfLines)] == line))
            {
                lines[static_cast<std::size_t>(numOfLines)] = line;
                buildVertexBuffer = true;
            }
        }
        else
        {
            lines.push_back(line);
            buildVertexBuffer = true;
        }
        ++numOfLines;
    }

    void LineManager2D::AddLineWithShadow(
        const Point startPoint, const Point endPoint, const Color color)
    {
        AddLine(Point(startPoint.X, startPoint.Y + 1),
                Point(endPoint.X, endPoint.Y + 1), Color::Black);
        AddLine(startPoint, endPoint, color);
    }

    void LineManager2D::UpdateVertexBuffer()
    {
        if (numOfLines == 0 ||
            lines.size() < static_cast<std::size_t>(numOfLines))
        {
            numOfPrimitives = 0;
            return;
        }
        for (int lineNumber = 0; lineNumber < numOfLines; ++lineNumber)
        {
            const Line& line = lines[static_cast<std::size_t>(lineNumber)];
            lineVertices[static_cast<std::size_t>(lineNumber * 2)] =
                VertexPositionColor(Vector3(
                    -1.0f + 2.0f * line.startPoint.X /
                        mapper.getWidthProperty(),
                    1.0f - 2.0f * line.startPoint.Y /
                        mapper.getHeightProperty(),
                    0.0f), line.color);
            lineVertices[static_cast<std::size_t>(lineNumber * 2 + 1)] =
                VertexPositionColor(Vector3(
                    -1.0f + 2.0f * line.endPoint.X /
                        mapper.getWidthProperty(),
                    1.0f - 2.0f * line.endPoint.Y /
                        mapper.getHeightProperty(),
                    0.0f), line.color);
        }
        numOfPrimitives = numOfLines;
        buildVertexBuffer = false;
    }

    void LineManager2D::Render()
    {
        if (buildVertexBuffer || numOfPrimitives != numOfLines)
            UpdateVertexBuffer();
        lastPrimitiveCount = numOfPrimitives;
        if (numOfPrimitives > 0)
        {
            device.setDepthStencilStateProperty(DepthStencilState::None);
            device.setBlendStateProperty(BlendState::AlphaBlend);
            EffectTechnique* technique =
                effect->getTechniquesProperty()["LineRendering2D"];
            effect->setCurrentTechniqueProperty(technique);
            for (EffectPass& pass : technique->getPassesProperty())
            {
                pass.Apply();
                device.DrawUserPrimitives(
                    PrimitiveType::LineList, lineVertices.data(), 0,
                    numOfPrimitives,
                    VertexPositionColor::getVertexDeclarationStatic());
            }
            device.setDepthStencilStateProperty(DepthStencilState::Default);
        }
        numOfLines = 0;
    }

    int LineManager2D::getLastPrimitiveCountProperty() const
    {
        return lastPrimitiveCount;
    }
}
