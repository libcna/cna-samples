// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SkinningData.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "AnimationClip.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/Object.hpp"

namespace SkinnedModel
{
    using Microsoft::Xna::Framework::Matrix;

    /** @brief Animation clips keyed by their content-pipeline name. */
    using AnimationClipDictionary =
        std::unordered_map<std::string, std::shared_ptr<AnimationClip>>;

    /** @brief Skeleton bone indices keyed by their content-pipeline names. */
    using BoneIndexDictionary = std::unordered_map<std::string, int>;

    /** @brief Combines all data needed to render and animate a skinned model. */
    class SkinningData : public System::Object
    {
    public:
        /**
         * @brief Constructs skinning data.
         * @param animationClips Animation clips keyed by name.
         * @param bindPose Bind-pose transforms relative to parent bones.
         * @param inverseBindPose Vertex-to-bone-space transforms.
         * @param skeletonHierarchy Parent index for each skeleton bone.
         * @param boneIndices Skeleton bone indices keyed by name.
         */
        SkinningData(AnimationClipDictionary animationClips,
                     std::vector<Matrix> bindPose,
                     std::vector<Matrix> inverseBindPose,
                     std::vector<int> skeletonHierarchy,
                     BoneIndexDictionary boneIndices);

        /** @brief Constructs empty skinning data for the XNB deserializer. */
        SkinningData() = default;

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `SkinnedModel.SkinningData`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets animation clips keyed by name.
         * @return Animation clip dictionary.
         */
        [[nodiscard]] const AnimationClipDictionary& getAnimationClipsProperty() const
        {
            return animationClips;
        }

        /**
         * @brief Gets bind-pose transforms relative to parent bones.
         * @return Bind-pose transforms.
         */
        [[nodiscard]] const std::vector<Matrix>& getBindPoseProperty() const { return bindPose; }

        /**
         * @brief Gets vertex-to-bone-space transforms.
         * @return Inverse bind-pose transforms.
         */
        [[nodiscard]] const std::vector<Matrix>& getInverseBindPoseProperty() const
        {
            return inverseBindPose;
        }

        /**
         * @brief Gets each bone's parent index.
         * @return Skeleton hierarchy.
         */
        [[nodiscard]] const std::vector<int>& getSkeletonHierarchyProperty() const
        {
            return skeletonHierarchy;
        }

        /**
         * @brief Gets skeleton bone indices keyed by name.
         * @return Bone-name lookup table.
         */
        [[nodiscard]] const BoneIndexDictionary& getBoneIndicesProperty() const
        {
            return boneIndices;
        }

    private:
        friend struct SkinningContentReaderRegistrationEXT;

        AnimationClipDictionary animationClips;
        std::vector<Matrix> bindPose;
        std::vector<Matrix> inverseBindPose;
        std::vector<int> skeletonHierarchy;
        BoneIndexDictionary boneIndices;
    };
}
