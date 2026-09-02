// SPDX-License-Identifier: MS-PL

#include "Helpers/Vector3Helper.hpp"

#include <cmath>

namespace RacingGame::Helpers
{
    using Microsoft::Xna::Framework::Vector3;

    float Vector3Helper::GetAngleBetweenVectors(Vector3 vec1, Vector3 vec2)
    {
        return static_cast<float>(std::acos(Vector3::Dot(vec1, vec2)));
    }

    float Vector3Helper::DistanceToLine(
        Vector3 point, Vector3 linePos1, Vector3 linePos2)
    {
        const Vector3 lineVec = linePos2 - linePos1;
        const Vector3 pointVec = linePos1 - point;
        return Vector3::Cross(lineVec, pointVec).Length() / lineVec.Length();
    }

    float Vector3Helper::SignedDistanceToPlane(
        Vector3 point, Vector3 planePosition, Vector3 planeNormal)
    {
        const Vector3 pointVec = planePosition - point;
        return Vector3::Dot(planeNormal, pointVec);
    }
}
