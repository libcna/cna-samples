// SPDX-License-Identifier: MS-PL

#include "RectangleExtensions.hpp"

#include <cmath>

namespace Platformer
{
    Microsoft::Xna::Framework::Vector2 RectangleExtensions::GetIntersectionDepth(
        const Microsoft::Xna::Framework::Rectangle rectA,
        const Microsoft::Xna::Framework::Rectangle rectB)
    {
        using Microsoft::Xna::Framework::Vector2;

        const float halfWidthA = rectA.Width / 2.0f;
        const float halfHeightA = rectA.Height / 2.0f;
        const float halfWidthB = rectB.Width / 2.0f;
        const float halfHeightB = rectB.Height / 2.0f;
        const Vector2 centerA(rectA.getLeftProperty() + halfWidthA,
                              rectA.getTopProperty() + halfHeightA);
        const Vector2 centerB(rectB.getLeftProperty() + halfWidthB,
                              rectB.getTopProperty() + halfHeightB);
        const float distanceX = centerA.X - centerB.X;
        const float distanceY = centerA.Y - centerB.Y;
        const float minDistanceX = halfWidthA + halfWidthB;
        const float minDistanceY = halfHeightA + halfHeightB;

        if (std::abs(distanceX) >= minDistanceX || std::abs(distanceY) >= minDistanceY)
            return Vector2::Zero;

        const float depthX = distanceX > 0.0f
                                 ? minDistanceX - distanceX
                                 : -minDistanceX - distanceX;
        const float depthY = distanceY > 0.0f
                                 ? minDistanceY - distanceY
                                 : -minDistanceY - distanceY;
        return {depthX, depthY};
    }

    Microsoft::Xna::Framework::Vector2 RectangleExtensions::GetBottomCenter(
        const Microsoft::Xna::Framework::Rectangle rect)
    {
        return {rect.X + rect.Width / 2.0f, static_cast<float>(rect.getBottomProperty())};
    }
}
