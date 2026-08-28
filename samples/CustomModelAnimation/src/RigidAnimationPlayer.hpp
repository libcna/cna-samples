// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// RigidAnimationPlayer.cs
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
     * @brief Plays an animation on a rigid model, applying transformations to each of the objects
     *        in the model over time.
     */
    class RigidAnimationPlayer : public ModelAnimationPlayerBase
    {
    public:
        /**
         * @brief Creates a new rigid animation player.
         * @param count Number of bones (objects) in the model.
         * @throws System::Exception if @p count is not positive.
         */
        explicit RigidAnimationPlayer(int count);

        /**
         * @brief Returns the fully qualified logical type name of this object.
         * @return "CustomModelAnimation.RigidAnimationPlayer".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets the current bone transform matrices for the animation.
         * @return One transform per bone.
         */
        [[nodiscard]] const std::vector<Matrix>& GetBoneTransforms() const { return boneTransforms; }

    protected:
        /** @brief Initializes all the bone transforms to the identity. */
        void InitClip() override;

        /**
         * @brief Sets the key frame for a bone to a transform.
         * @param keyframe Keyframe to set.
         */
        void SetKeyframe(const ModelKeyframe& keyframe) override;

    private:
        // This is an array of the transforms to each object in the model
        std::vector<Matrix> boneTransforms;
    };
}
