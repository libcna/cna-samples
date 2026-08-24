// SPDX-License-Identifier: MS-PL

#include "Circle.hpp"

#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace Platformer
{
    Circle::Circle(const Microsoft::Xna::Framework::Vector2 position, const float radius)
        : Center(position), Radius(radius)
    {
    }

    bool Circle::Intersects(const Microsoft::Xna::Framework::Rectangle rectangle) const
    {
        using namespace Microsoft::Xna::Framework;
        const Vector2 nearest(
            MathHelper::Clamp(Center.X, static_cast<float>(rectangle.getLeftProperty()),
                              static_cast<float>(rectangle.getRightProperty())),
            MathHelper::Clamp(Center.Y, static_cast<float>(rectangle.getTopProperty()),
                              static_cast<float>(rectangle.getBottomProperty())));
        const Vector2 direction = Center - nearest;
        const float distanceSquared = direction.LengthSquared();
        return distanceSquared > 0.0f && distanceSquared < Radius * Radius;
    }
}
