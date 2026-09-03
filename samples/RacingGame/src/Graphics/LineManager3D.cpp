// SPDX-License-Identifier: MS-PL

#include "Graphics/LineManager3D.hpp"

#include <stdexcept>

#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectPass.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectTechnique.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"

namespace RacingGame::Graphics
{
    using Microsoft::Xna::Framework::Color;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Content::ContentManager;
    using namespace Microsoft::Xna::Framework::Graphics;

    LineManager3D::LineManager3D(
        GraphicsDevice& setDevice, ContentManager& content)
        : device(setDevice),
          effect(content.Load<std::shared_ptr<Effect>>(
              "Shaders/LineRendering"))
    {
        if (!effect || !effect->getTechniquesProperty()["LineRendering3D"])
            throw std::runtime_error(
                "Authentic Racing LineRendering3D effect failed to load");
    }

    void LineManager3D::AddLine(
        const Vector3 startPoint, const Color startColor,
        const Vector3 endPoint, const Color endColor)
    {
        if (numOfLines >= MaxNumOfLines) return;
        const Line line{startPoint, startColor, endPoint, endColor};
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

    void LineManager3D::AddLine(
        const Vector3 startPoint, const Vector3 endPoint, const Color color)
    {
        AddLine(startPoint, color, endPoint, color);
    }

    void LineManager3D::UpdateVertexBuffer()
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
                VertexPositionColor(line.startPoint, line.startColor);
            lineVertices[static_cast<std::size_t>(lineNumber * 2 + 1)] =
                VertexPositionColor(line.endPoint, line.endColor);
        }
        numOfPrimitives = numOfLines;
        buildVertexBuffer = false;
    }

    void LineManager3D::Render(
        const Matrix& world, const Matrix& view, const Matrix& projection)
    {
        if (buildVertexBuffer || numOfPrimitives != numOfLines)
            UpdateVertexBuffer();
        lastPrimitiveCount = numOfPrimitives;
        if (numOfPrimitives > 0)
        {
            auto& parameters = effect->getParametersProperty();
            if (EffectParameter* parameter = parameters["worldViewProj"])
                parameter->SetValue(world * view * projection);
            EffectTechnique* technique =
                effect->getTechniquesProperty()["LineRendering3D"];
            effect->setCurrentTechniqueProperty(technique);
            device.setBlendStateProperty(BlendState::AlphaBlend);
            for (EffectPass& pass : technique->getPassesProperty())
            {
                pass.Apply();
                device.DrawUserPrimitives(
                    PrimitiveType::LineList, lineVertices.data(), 0,
                    numOfPrimitives,
                    VertexPositionColor::getVertexDeclarationStatic());
            }
        }
        numOfLines = 0;
    }

    int LineManager3D::getLastPrimitiveCountProperty() const
    {
        return lastPrimitiveCount;
    }
}
