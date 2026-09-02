// SPDX-License-Identifier: MS-PL

#pragma once

#include "Microsoft/Xna/Framework/Vector3.hpp"

namespace RacingGame::Helpers
{
    /** @brief Provides vector calculations used by the Racing game. */
    class Vector3Helper final
    {
    public:
        /**
         * @brief Returns the angle between two normalized vectors.
         *
         * @param vec1 First normalized vector.
         * @param vec2 Second normalized vector.
         * @return The angle in radians.
         */
        [[nodiscard]] static float GetAngleBetweenVectors(
            Microsoft::Xna::Framework::Vector3 vec1,
            Microsoft::Xna::Framework::Vector3 vec2);

        /**
         * @brief Returns the distance from a point to an infinite line.
         *
         * @param point Point whose distance is measured.
         * @param linePos1 First point on the line.
         * @param linePos2 Second point on the line.
         * @return The perpendicular distance to the line.
         */
        [[nodiscard]] static float DistanceToLine(
            Microsoft::Xna::Framework::Vector3 point,
            Microsoft::Xna::Framework::Vector3 linePos1,
            Microsoft::Xna::Framework::Vector3 linePos2);

        /**
         * @brief Returns a point's signed distance along a plane normal.
         *
         * @param point Point whose distance is measured.
         * @param planePosition A position on the plane.
         * @param planeNormal Plane normal used for the signed projection.
         * @return The signed projection from the point to the plane.
         */
        [[nodiscard]] static float SignedDistanceToPlane(
            Microsoft::Xna::Framework::Vector3 point,
            Microsoft::Xna::Framework::Vector3 planePosition,
            Microsoft::Xna::Framework::Vector3 planeNormal);

    private:
        Vector3Helper() = delete;
    };
}
