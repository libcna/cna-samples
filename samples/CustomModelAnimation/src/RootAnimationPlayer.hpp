// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// RootAnimationPlayer.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "ModelAnimationPlayerBase.hpp"

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

namespace CustomModelAnimation
{
    using Microsoft::Xna::Framework::Matrix;

    /**
     * @brief An animation player that contains a single transformation, used to move, position or
     *        scale something.
     */
    class RootAnimationPlayer : public ModelAnimationPlayerBase
    {
    public:
        /**
         * @brief Returns the fully qualified logical type name of this object.
         * @return "CustomModelAnimation.RootAnimationPlayer".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets the current transformation being applied.
         * @return The transformation matrix.
         */
        [[nodiscard]] const Matrix& GetCurrentTransform() const { return currentTransform; }

    protected:
        /** @brief Initializes the transformation to the identity. */
        void InitClip() override;

        /**
         * @brief Sets the key frame by storing the current transform.
         * @param keyframe Keyframe to set.
         */
        void SetKeyframe(const ModelKeyframe& keyframe) override;

    private:
        Matrix currentTransform;
    };
}
