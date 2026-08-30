// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CpuSkinningHelpers.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "CpuSkinningHelpers.hpp"

#include <cstddef>

namespace CpuSkinningDataTypes
{
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::Vector3;
    using Microsoft::Xna::Framework::Vector4;

    void CpuSkinningHelpers::SkinVertex(
        const std::vector<Matrix>& bones,
        const Vector3& position,
        const Vector3& normal,
        const Vector4& blendIndices,
        const Vector4& blendWeights,
        Vector3& outPosition,
        Vector3& outNormal)
    {
        const int b0 = static_cast<int>(blendIndices.X);
        const int b1 = static_cast<int>(blendIndices.Y);
        const int b2 = static_cast<int>(blendIndices.Z);
        const int b3 = static_cast<int>(blendIndices.W);

        Matrix skinnedTransformSum;
        Blend4x3Matrix(
            bones.at(static_cast<std::size_t>(b0)),
            bones.at(static_cast<std::size_t>(b1)),
            bones.at(static_cast<std::size_t>(b2)),
            bones.at(static_cast<std::size_t>(b3)),
            blendWeights,
            skinnedTransformSum);

        outPosition = Vector3::Transform(position, skinnedTransformSum);
        outNormal = Vector3::TransformNormal(normal, skinnedTransformSum);
    }

    void CpuSkinningHelpers::Blend4x3Matrix(
        const Matrix& m1,
        const Matrix& m2,
        const Matrix& m3,
        const Matrix& m4,
        const Vector4& weights,
        Matrix& blended)
    {
        const float w1 = weights.X;
        const float w2 = weights.Y;
        const float w3 = weights.Z;
        const float w4 = weights.W;

        const float num11 = (m1.M11 * w1) + (m2.M11 * w2) + (m3.M11 * w3) + (m4.M11 * w4);
        const float num12 = (m1.M12 * w1) + (m2.M12 * w2) + (m3.M12 * w3) + (m4.M12 * w4);
        const float num13 = (m1.M13 * w1) + (m2.M13 * w2) + (m3.M13 * w3) + (m4.M13 * w4);
        const float num21 = (m1.M21 * w1) + (m2.M21 * w2) + (m3.M21 * w3) + (m4.M21 * w4);
        const float num22 = (m1.M22 * w1) + (m2.M22 * w2) + (m3.M22 * w3) + (m4.M22 * w4);
        const float num23 = (m1.M23 * w1) + (m2.M23 * w2) + (m3.M23 * w3) + (m4.M23 * w4);
        const float num31 = (m1.M31 * w1) + (m2.M31 * w2) + (m3.M31 * w3) + (m4.M31 * w4);
        const float num32 = (m1.M32 * w1) + (m2.M32 * w2) + (m3.M32 * w3) + (m4.M32 * w4);
        const float num33 = (m1.M33 * w1) + (m2.M33 * w2) + (m3.M33 * w3) + (m4.M33 * w4);
        const float num41 = (m1.M41 * w1) + (m2.M41 * w2) + (m3.M41 * w3) + (m4.M41 * w4);
        const float num42 = (m1.M42 * w1) + (m2.M42 * w2) + (m3.M42 * w3) + (m4.M42 * w4);
        const float num43 = (m1.M43 * w1) + (m2.M43 * w2) + (m3.M43 * w3) + (m4.M43 * w4);

        blended.M11 = num11;
        blended.M12 = num12;
        blended.M13 = num13;
        blended.M14 = 0.0f;
        blended.M21 = num21;
        blended.M22 = num22;
        blended.M23 = num23;
        blended.M24 = 0.0f;
        blended.M31 = num31;
        blended.M32 = num32;
        blended.M33 = num33;
        blended.M34 = 0.0f;
        blended.M41 = num41;
        blended.M42 = num42;
        blended.M43 = num43;
        blended.M44 = 1.0f;
    }
}
