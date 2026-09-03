// SPDX-License-Identifier: MS-PL

#include "Graphics/ResolutionMapper.hpp"

#include <algorithm>
#include <cmath>

#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"

namespace RacingGame::Graphics
{
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;

    ResolutionMapper::ResolutionMapper(GraphicsDevice& setDevice)
        : device(setDevice)
    {
    }

    int ResolutionMapper::Round(const double value)
    {
        return static_cast<int>(std::nearbyint(value));
    }

    int ResolutionMapper::getWidthProperty() const
    {
        return device.getViewportProperty().getWidthProperty();
    }

    int ResolutionMapper::getHeightProperty() const
    {
        return device.getViewportProperty().getHeightProperty();
    }

    int ResolutionMapper::XToRes(const int value) const
    {
        return Round(value * getWidthProperty() / 1024.0f);
    }

    int ResolutionMapper::YToRes(const int value) const
    {
        return Round(value * getHeightProperty() / 640.0f);
    }

    int ResolutionMapper::YToRes768(const int value) const
    {
        return Round(value * getHeightProperty() / 768.0f);
    }

    int ResolutionMapper::XToRes1600(const int value) const
    {
        return Round(value * getWidthProperty() / 1600.0f);
    }

    int ResolutionMapper::YToRes1200(const int value) const
    {
        return Round(value * getHeightProperty() / 1200.0f);
    }

    int ResolutionMapper::XToRes1400(const int value) const
    {
        return Round(value * getWidthProperty() / 1400.0f);
    }

    int ResolutionMapper::YToRes1050(const int value) const
    {
        return Round(value * getHeightProperty() / 1050.0f);
    }

    Rectangle ResolutionMapper::CalcRectangle(
        const int x, const int y, const int width, const int height) const
    {
        const float widthFactor = getWidthProperty() / 1024.0f;
        const float heightFactor = getHeightProperty() / 640.0f;
        return {Round(x * widthFactor), Round(y * heightFactor),
                Round(width * widthFactor), Round(height * heightFactor)};
    }

    Rectangle ResolutionMapper::CalcRectangleWithBounce(
        const int x, const int y, const int width, const int height,
        const float bounce) const
    {
        const float widthFactor = getWidthProperty() / 1024.0f;
        const float heightFactor = getHeightProperty() / 640.0f;
        const float middleX = (x + width / 2) * widthFactor;
        const float middleY = (y + height / 2) * heightFactor;
        const float outputWidth = width * widthFactor * bounce;
        const float outputHeight = height * heightFactor * bounce;
        return {Round(middleX - outputWidth / 2.0f),
                Round(middleY - outputHeight / 2.0f), Round(outputWidth),
                Round(outputHeight)};
    }

    Rectangle ResolutionMapper::CalcRectangleKeep4To3(
        const int x, const int y, const int width, const int height) const
    {
        const float widthFactor = getWidthProperty() / 1024.0f;
        const float heightFactor = getHeightProperty() / 768.0f;
        return {Round(x * widthFactor), Round(y * heightFactor),
                Round(width * widthFactor), Round(height * heightFactor)};
    }

    Rectangle ResolutionMapper::CalcRectangleKeep4To3(
        const Rectangle& rectangle) const
    {
        return CalcRectangleKeep4To3(
            rectangle.X, rectangle.Y, rectangle.Width, rectangle.Height);
    }

    Rectangle ResolutionMapper::CalcRectangle1600(
        const int x, const int y, const int width, const int height) const
    {
        const float widthFactor = getWidthProperty() / 1600.0f;
        const float heightFactor = getHeightProperty() / 1200.0f;
        return {Round(x * widthFactor), Round(y * heightFactor),
                Round(width * widthFactor), Round(height * heightFactor)};
    }

    Rectangle ResolutionMapper::CalcRectangle2000(
        const int x, const int y, const int width, const int height) const
    {
        const float widthFactor = getWidthProperty() / 2000.0f;
        const float heightFactor = getHeightProperty() / 1500.0f;
        return {Round(x * widthFactor), Round(y * heightFactor),
                Round(width * widthFactor), Round(height * heightFactor)};
    }

    Rectangle ResolutionMapper::CalcRectangleKeep4To3AlignBottom(
        const int x, const int y, const int width, const int height) const
    {
        const float widthFactor = getWidthProperty() / 1024.0f;
        const float heightFactor16To9 = getHeightProperty() / 640.0f;
        const float heightFactor4To3 = getHeightProperty() / 768.0f;
        return {static_cast<int>(x * widthFactor),
                static_cast<int>(y * heightFactor16To9) -
                    Round(height * heightFactor4To3),
                Round(width * widthFactor), Round(height * heightFactor4To3)};
    }

    Rectangle ResolutionMapper::CalcRectangleKeep4To3AlignBottomRight(
        const int x, const int y, const int width, const int height) const
    {
        const float widthFactor = getWidthProperty() / 1024.0f;
        const float heightFactor16To9 = getHeightProperty() / 640.0f;
        const float heightFactor4To3 = getHeightProperty() / 768.0f;
        return {static_cast<int>(x * widthFactor) - Round(width * widthFactor),
                static_cast<int>(y * heightFactor16To9) -
                    Round(height * heightFactor4To3),
                Round(width * widthFactor), Round(height * heightFactor4To3)};
    }

    Rectangle ResolutionMapper::CalcRectangleCenteredWithGivenHeight(
        const int x, const int y, const int height,
        const Rectangle& source) const
    {
        const float widthFactor = getWidthProperty() / 1024.0f;
        const float heightFactor = getHeightProperty() / 640.0f;
        const int rectangleHeight = Round(height * heightFactor);
        const int rectangleWidth = Round(
            source.Width * rectangleHeight /
            static_cast<float>(source.Height));
        return {
            std::max(0, Round(x * widthFactor) - rectangleWidth / 2),
            std::max(0, Round(y * heightFactor) - rectangleHeight / 2),
            rectangleWidth, rectangleHeight};
    }
}
