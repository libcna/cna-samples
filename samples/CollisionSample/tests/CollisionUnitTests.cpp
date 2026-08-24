#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "BoundingOrientedBox.hpp"
#include "TriangleTest.hpp"
#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/BoundingSphere.hpp"
#include "Microsoft/Xna/Framework/ContainmentType.hpp"
#include "Microsoft/Xna/Framework/Plane.hpp"
#include "Microsoft/Xna/Framework/Quaternion.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "System/Random.hpp"

namespace UnitTests {

using namespace CollisionSample;
using namespace Microsoft::Xna::Framework;

float NextFloat(System::Random& random, float min, float max) {
    return static_cast<float>(random.NextDouble()) * (max - min) + min;
}

Vector3 PointInCube(System::Random& random) {
    return Vector3(
        NextFloat(random, -1.0f, 1.0f),
        NextFloat(random, -1.0f, 1.0f),
        NextFloat(random, -1.0f, 1.0f));
}

Vector3 PointInSphere(System::Random& random) {
    for (;;) {
        Vector3 point = PointInCube(random);
        if (point.LengthSquared() <= 1.0f) {
            point.Normalize();
            return point;
        }
    }
}

Quaternion Orientation(System::Random& random) {
    for (;;) {
        Quaternion orientation(
            NextFloat(random, -1.0f, 1.0f),
            NextFloat(random, -1.0f, 1.0f),
            NextFloat(random, -1.0f, 1.0f),
            NextFloat(random, -1.0f, 1.0f));
        if (orientation.LengthSquared() <= 1.0f) {
            orientation.Normalize();
            return orientation;
        }
    }
}

struct BoundingSphereOps {
    BoundingSphere Shape;

    void CreateRandomShape(System::Random& random, float scale) {
        Shape.Radius = NextFloat(random, 0.0f, scale);
        Shape.Center = PointInCube(random) * scale;
    }

    void Translate(Vector3 translation) { Shape.Center += translation; }

    void Scale(float scale) {
        Shape.Radius *= scale;
        Shape.Center = Shape.Center * scale;
    }

    Vector3 RandomInteriorPoint(System::Random& random) const {
        return PointInSphere(random) * Shape.Radius + Shape.Center;
    }

    bool ContainsPoint(Vector3 point) const {
        return Shape.Contains(point) != ContainmentType::Disjoint;
    }

    std::vector<Vector3> GetHull() const {
        const Vector3 c = Shape.Center;
        const float r = Shape.Radius;
        return {
            c + Vector3(r, r, r), c + Vector3(-r, r, r),
            c + Vector3(r, -r, r), c + Vector3(-r, -r, r),
            c + Vector3(r, r, -r), c + Vector3(-r, r, -r),
            c + Vector3(r, -r, -r), c + Vector3(-r, -r, -r),
        };
    }

    float MinimumDistanceFromPlane(const Plane& plane) const {
        return plane.DotCoordinate(Shape.Center) - Shape.Radius;
    }
};

struct TriangleOps {
    Triangle Shape;

    void CreateRandomShape(System::Random& random, float scale) {
        Shape.V0 = PointInCube(random) * scale;
        Shape.V1 = PointInCube(random) * scale;
        Shape.V2 = PointInCube(random) * scale;
    }

    void Translate(Vector3 translation) {
        Shape.V0 += translation;
        Shape.V1 += translation;
        Shape.V2 += translation;
    }

    void Scale(float scale) {
        Shape.V0 = Shape.V0 * scale;
        Shape.V1 = Shape.V1 * scale;
        Shape.V2 = Shape.V2 * scale;
    }

    Vector3 RandomInteriorPoint(System::Random& random) const {
        float u = static_cast<float>(random.NextDouble());
        float v = static_cast<float>(random.NextDouble());
        if (u + v > 1.0f) {
            u = 1.0f - u;
            v = 1.0f - v;
        }
        return (1.0f - u - v) * Shape.V0 + u * Shape.V1 + v * Shape.V2;
    }

    std::vector<Vector3> GetHull() const { return {Shape.V0, Shape.V1, Shape.V2}; }

    float MinimumDistanceFromPlane(const Plane& plane) const {
        float distance = std::numeric_limits<float>::max();
        for (const Vector3& point : GetHull())
            distance = std::min(distance, plane.DotCoordinate(point));
        return distance;
    }
};

struct BoundingOrientedBoxOps {
    BoundingOrientedBox Shape;

    void CreateRandomShape(System::Random& random, float scale) {
        Shape.Center = PointInCube(random) * scale;
        Shape.HalfExtent = (PointInCube(random) + Vector3::One * 1.01f) * scale * 0.25f;
        Shape.Orientation = Orientation(random);
    }

    void Translate(Vector3 translation) { Shape.Center += translation; }

    void Scale(float scale) {
        Shape.HalfExtent = Shape.HalfExtent * scale;
        Shape.Center = Shape.Center * scale;
    }

    Vector3 RandomInteriorPoint(System::Random& random) const {
        Vector3 point(
            NextFloat(random, -Shape.HalfExtent.X, Shape.HalfExtent.X),
            NextFloat(random, -Shape.HalfExtent.Y, Shape.HalfExtent.Y),
            NextFloat(random, -Shape.HalfExtent.Z, Shape.HalfExtent.Z));
        point = Vector3::Transform(point, Shape.Orientation);
        return point + Shape.Center;
    }

    bool ContainsPoint(Vector3 point) const { return Shape.Contains(point); }

    std::vector<Vector3> GetHull() const { return Shape.GetCorners(); }

    float MinimumDistanceFromPlane(const Plane& plane) const {
        float distance = std::numeric_limits<float>::max();
        for (const Vector3& point : GetHull())
            distance = std::min(distance, plane.DotCoordinate(point));
        return distance;
    }
};

struct BoundingBoxOps {
    BoundingBox Shape;

    void CreateRandomShape(System::Random& random, float scale) {
        const Vector3 center = PointInCube(random) * scale;
        const Vector3 size = (PointInCube(random) + Vector3::One * 1.01f) * scale * 0.25f;
        Shape.Min = center - size;
        Shape.Max = center + size;
    }

    void Translate(Vector3 translation) {
        Shape.Min += translation;
        Shape.Max += translation;
    }

    void Scale(float scale) {
        Shape.Min = Shape.Min * scale;
        Shape.Max = Shape.Max * scale;
    }

    Vector3 RandomInteriorPoint(System::Random& random) const {
        return Vector3(
            NextFloat(random, Shape.Min.X, Shape.Max.X),
            NextFloat(random, Shape.Min.Y, Shape.Max.Y),
            NextFloat(random, Shape.Min.Z, Shape.Max.Z));
    }

    bool ContainsPoint(Vector3 point) const {
        return Shape.Contains(point) != ContainmentType::Disjoint;
    }

    std::vector<Vector3> GetHull() const { return Shape.GetCorners(); }

    float MinimumDistanceFromPlane(const Plane& plane) const {
        float distance = std::numeric_limits<float>::max();
        for (const Vector3& point : GetHull())
            distance = std::min(distance, plane.DotCoordinate(point));
        return distance;
    }
};

class CollisionUnitTests {
public:
    int TestsFailed = 0;
    int TestsPassed = 0;

    void ValidatePortSurface() {
        const BoundingOrientedBox box(
            Vector3(1.0f, 2.0f, 3.0f),
            Vector3(4.0f, 5.0f, 6.0f),
            Quaternion::Identity);
        const std::vector<Vector3> corners = box.GetCorners();
        std::vector<Vector3> offsetCorners(10, Vector3::Zero);
        box.GetCorners(offsetCorners, 1);

        bool valid = box.GetHashCode() == box.GetHashCode();
        valid = valid && box.ToString() ==
            "{Center:" + box.Center.ToString() +
            " Extents:" + box.HalfExtent.ToString() +
            " Orientation:" + box.Orientation.ToString() + "}";
        valid = valid && offsetCorners.front() == Vector3::Zero;
        for (std::size_t i = 0; i < corners.size(); ++i)
            valid = valid && offsetCorners[i + 1] == corners[i];

        bool rejectedShortDestination = false;
        try {
            std::vector<Vector3> shortDestination(BoundingOrientedBox::CornerCount);
            box.GetCorners(shortDestination, 1);
        } catch (const std::out_of_range&) {
            rejectedShortDestination = true;
        }
        valid = valid && rejectedShortDestination;

        if (!valid) {
            TestsFailed++;
            std::cout << "Failed BoundingOrientedBox surface parity" << std::endl;
        }
    }

    template<typename Ops1, typename Ops2, typename Checker, typename Wanted>
    void Check(const std::string& message, const Ops1& object1, const Ops2& object2,
               Checker checker, Wanted wanted) {
        if (wanted(checker(object1.Shape, object2.Shape))) {
            TestsPassed++;
        } else {
            TestsFailed++;
            std::cout << "Failed " << message << std::endl;
            checker(object1.Shape, object2.Shape);
        }
    }

    template<typename Ops1, typename Ops2, typename Contains, typename Intersects>
    void TestRandomObjects(System::Random& random, Ops1 object1, Ops2 object2,
                           Contains contains, Intersects intersects) {
        object1.CreateRandomShape(random, static_cast<float>(std::exp(random.NextDouble() * 4.0 - 2.0)));
        object2.CreateRandomShape(random, static_cast<float>(std::exp(random.NextDouble() * 4.0 - 2.0)));

        Vector3 point1 = object1.RandomInteriorPoint(random);
        Vector3 point2 = object2.RandomInteriorPoint(random);

        object2.Translate(point1 - point2);
        Check("Contains", object1, object2, contains,
              [](ContainmentType value) { return value != ContainmentType::Disjoint; });
        Check("Intersects", object1, object2, intersects, [](bool value) { return value; });

        point2 = object2.RandomInteriorPoint(random);
        while (object1.ContainsPoint(point2)) {
            point2 = point1 + (point2 - point1) * 1.1f;
            object2.Translate(-point1);
            object2.Scale(1.1f);
            object2.Translate(point1);
        }
        Check("surface Intersects", object1, object2, contains,
              [](ContainmentType value) { return value == ContainmentType::Intersects; });
        Check("Intersects", object1, object2, intersects, [](bool value) { return value; });

        for (;;) {
            bool allInside = true;
            for (const Vector3& point : object2.GetHull()) {
                if (!object1.ContainsPoint(point)) {
                    allInside = false;
                    break;
                }
            }
            if (allInside)
                break;

            point1 = object1.RandomInteriorPoint(random);
            object2.Translate(-point1);
            object2.Scale(0.9f);
            object2.Translate(point1);
        }

        Plane plane;
        plane.Normal = Vector3::Normalize(PointInSphere(random));
        plane.D = static_cast<float>(random.NextDouble() * 2.0 - 1.0) * 100.0f;

        object1.Translate((-object1.MinimumDistanceFromPlane(plane) + 0.001f) * plane.Normal);
        plane.D = -plane.D;
        plane.Normal = -plane.Normal;
        object2.Translate((-object2.MinimumDistanceFromPlane(plane) + 0.001f) * plane.Normal);

        Check("Disjoint", object1, object2, contains,
              [](ContainmentType value) { return value == ContainmentType::Disjoint; });
        Check("!Intersects", object1, object2, intersects, [](bool value) { return !value; });
    }

    void RunTests() {
        System::Random random(1);
        for (int i = 0; i < 10000; i++) {
            TestRandomObjects(random, BoundingOrientedBoxOps{}, BoundingOrientedBoxOps{},
                [](const BoundingOrientedBox& box1, const BoundingOrientedBox& box2) { return box1.Contains(box2); },
                [](const BoundingOrientedBox& box1, const BoundingOrientedBox& box2) { return box1.Intersects(box2); });

            TestRandomObjects(random, BoundingOrientedBoxOps{}, BoundingBoxOps{},
                [](const BoundingOrientedBox& box1, const BoundingBox& box2) { return box1.Contains(box2); },
                [](const BoundingOrientedBox& box1, const BoundingBox& box2) { return box1.Intersects(box2); });

            TestRandomObjects(random, BoundingBoxOps{}, BoundingOrientedBoxOps{},
                [](const BoundingBox& box1, const BoundingOrientedBox& box2) { return BoundingOrientedBox::Contains(box1, box2); },
                [](const BoundingBox& box1, const BoundingOrientedBox& box2) { return box2.Intersects(box1); });

            TestRandomObjects(random, BoundingBoxOps{}, BoundingBoxOps{},
                [](const BoundingBox& box1, const BoundingBox& box2) { return box1.Contains(box2); },
                [](const BoundingBox& box1, const BoundingBox& box2) { return box1.Intersects(box2); });

            TestRandomObjects(random, BoundingSphereOps{}, TriangleOps{},
                [](const BoundingSphere& sphere, const Triangle& triangle) { return TriangleTest::Contains(sphere, triangle); },
                [](const BoundingSphere& sphere, const Triangle& triangle) { return TriangleTest::Intersects(sphere, triangle); });

            TestRandomObjects(random, BoundingBoxOps{}, TriangleOps{},
                [](const BoundingBox& box, const Triangle& triangle) { return TriangleTest::Contains(box, triangle); },
                [](const BoundingBox& box, const Triangle& triangle) {
                    return TriangleTest::Intersects(box, triangle.V0, triangle.V1, triangle.V2);
                });

            TestRandomObjects(random, BoundingOrientedBoxOps{}, TriangleOps{},
                [](const BoundingOrientedBox& box, const Triangle& triangle) {
                    return TriangleTest::Contains(box, triangle);
                },
                [](const BoundingOrientedBox& box, const Triangle& triangle) {
                    return TriangleTest::Intersects(box, triangle.V0, triangle.V1, triangle.V2);
                });
        }

        ValidatePortSurface();

        std::cout << "Passed: " << TestsPassed << " Failed: " << TestsFailed << std::endl;
    }
};

} // namespace UnitTests

int main() {
    UnitTests::CollisionUnitTests tests;
    tests.RunTests();
    return tests.TestsFailed == 0 ? 0 : 1;
}
