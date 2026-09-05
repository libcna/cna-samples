// SPDX-License-Identifier: MS-PL
#pragma once

// ExtensionMethods.hpp — C++ port of Utility/ExtensionMethods.cs (XNA 4.0
// NinjAcademy sample). C# extension methods become free functions in the
// sample's own namespace, so the call sites keep the original names.

#include "Microsoft/Xna/Framework/BoundingBox.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace NinjAcademy {

using Microsoft::Xna::Framework::BoundingBox;
using Microsoft::Xna::Framework::Point;
using Microsoft::Xna::Framework::Vector2;

// Returns a vector pointing to the specified point.
inline Vector2 GetVector(const Point& point) {
    return Vector2((float)point.X, (float)point.Y);
}

// Returns the height of a bounding box.
inline float Height(const BoundingBox& box) {
    return box.Max.Y - box.Min.Y;
}

// Returns the width of a bounding box.
inline float Width(const BoundingBox& box) {
    return box.Max.X - box.Min.X;
}

} // namespace NinjAcademy
