// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CpuSkinningHelpers.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <vector>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace CpuSkinningDataTypes
{
    /** @brief Provides the sample's four-influence CPU vertex skinning algorithm. */
    class CpuSkinningHelpers
    {
    public:
        /** @brief Static utility type; instances cannot be constructed. */
        CpuSkinningHelpers() = delete;

        /**
         * @brief Skins one position and normal with four weighted bone transforms.
         *
         * @param bones Current skin transforms indexed by @p blendIndices.
         * @param position Bind-pose vertex position.
         * @param normal Bind-pose vertex normal.
         * @param blendIndices Four bone indices encoded in a Vector4.
         * @param blendWeights Four corresponding blend weights.
         * @param outPosition Receives the skinned position.
         * @param outNormal Receives the skinned normal.
         */
        static void SkinVertex(
            const std::vector<Microsoft::Xna::Framework::Matrix>& bones,
            const Microsoft::Xna::Framework::Vector3& position,
            const Microsoft::Xna::Framework::Vector3& normal,
            const Microsoft::Xna::Framework::Vector4& blendIndices,
            const Microsoft::Xna::Framework::Vector4& blendWeights,
            Microsoft::Xna::Framework::Vector3& outPosition,
            Microsoft::Xna::Framework::Vector3& outNormal);

    private:
        static void Blend4x3Matrix(
            const Microsoft::Xna::Framework::Matrix& m1,
            const Microsoft::Xna::Framework::Matrix& m2,
            const Microsoft::Xna::Framework::Matrix& m3,
            const Microsoft::Xna::Framework::Matrix& m4,
            const Microsoft::Xna::Framework::Vector4& weights,
            Microsoft::Xna::Framework::Matrix& blended);
    };
}
