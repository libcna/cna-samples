// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SkinnedAnimationPlayer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>

#include "ModelAnimationPlayerBase.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

namespace CustomModelAnimation
{
    using Microsoft::Xna::Framework::Matrix;

    /**
     * @brief The animation player that manipulates a skinned model.
     */
    class SkinnedAnimationPlayer : public ModelAnimationPlayerBase
    {
    public:
        /**
         * @brief Constructs a new animation player.
         *
         * @param bindPose Bind-pose matrices for each bone, relative to the parent bone.
         * @param inverseBindPose Vertex-to-bone-space transforms for each bone.
         * @param skeletonHierarchy For each bone, the index of its parent bone.
         * @throws System::Exception if @p bindPose is empty.
         */
        SkinnedAnimationPlayer(const std::vector<Matrix>& bindPose,
                               const std::vector<Matrix>& inverseBindPose,
                               const std::vector<int>& skeletonHierarchy);

        /**
         * @brief Returns the fully qualified logical type name of this object.
         * @return "CustomModelAnimation.SkinnedAnimationPlayer".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets the current bone transform matrices, relative to the skinning bind pose.
         * @return One skinning transform per bone.
         */
        [[nodiscard]] const std::vector<Matrix>& GetSkinTransforms() const { return skinTransforms; }

    protected:
        /** @brief Initializes the animation clip by copying the bind pose into the bone transforms. */
        void InitClip() override;

        /**
         * @brief Sets the key frame for the passed in frame.
         * @param keyframe Keyframe to set.
         */
        void SetKeyframe(const ModelKeyframe& keyframe) override;

        /** @brief Updates the transformations ultimately needed for rendering. */
        void OnUpdate() override;

    private:
        // Current animation transform matrices.
        std::vector<Matrix> boneTransforms;
        std::vector<Matrix> worldTransforms;
        std::vector<Matrix> skinTransforms;

        // The C# holds references to the lists the ModelData owns; the model outlives the player
        // in this sample, so this port copies them rather than tracking that lifetime.
        std::vector<Matrix> bindPose;
        std::vector<Matrix> inverseBindPose;
        std::vector<int> skeletonHierarchy;

        /**
         * @brief Gets the current bone transform matrices, relative to their parent bones.
         * @return One transform per bone.
         */
        [[nodiscard]] const std::vector<Matrix>& GetBoneTransforms() const { return boneTransforms; }

        /**
         * @brief Gets the current bone transform matrices, in absolute format.
         * @return One transform per bone.
         */
        [[nodiscard]] const std::vector<Matrix>& GetWorldTransforms() const { return worldTransforms; }
    };
}
