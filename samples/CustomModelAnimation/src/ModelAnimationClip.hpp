// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ModelAnimationClip.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "ModelKeyframe.hpp"

#include "CNA/CNAHelper.hpp"
#include "System/Object.hpp"
#include "System/TimeSpan.hpp"

namespace CustomModelAnimation
{
    /**
     * @brief A model animation clip is the runtime equivalent of the
     *        `Microsoft.Xna.Framework.Content.Pipeline.Graphics.AnimationContent` type.
     *
     * It holds all the keyframes needed to describe a single model animation.
     */
    class ModelAnimationClip : public System::Object
    {
    public:
        /**
         * @brief Constructs a new model animation clip object.
         *
         * @param duration The total length of the clip.
         * @param keyframes Every keyframe for every bone, sorted by time.
         */
        ModelAnimationClip(System::TimeSpan duration,
                           std::vector<std::shared_ptr<ModelKeyframe>> keyframes);

        /**
         * @brief Constructs an empty clip for the XNB deserializer to fill in.
         *
         * Public for the same reason @ref ModelKeyframe's is.
         */
        ModelAnimationClip() = default;

        /**
         * @brief Returns the fully qualified logical type name of this object.
         * @return "CustomModelAnimation.ModelAnimationClip".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets the total length of the model animation clip.
         * @return The clip's duration.
         */
        [[nodiscard]] System::TimeSpan getDurationProperty() const { return duration; }

        /**
         * @brief Gets a combined list containing all the keyframes for all bones, sorted by time.
         * @return The keyframes.
         */
        [[nodiscard]] const std::vector<std::shared_ptr<ModelKeyframe>>& getKeyframesProperty() const
        {
            return keyframes;
        }

    private:
        friend struct ModelContentReaderRegistrationEXT;

        System::TimeSpan duration;
        std::vector<std::shared_ptr<ModelKeyframe>> keyframes;
    };
}
