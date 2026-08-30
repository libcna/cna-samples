// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AnimationClip.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Keyframe.hpp"

#include "CNA/CNAHelper.hpp"
#include "System/Object.hpp"
#include "System/TimeSpan.hpp"

namespace CpuSkinningDataTypes
{
    /** @brief A combined reference-type keyframe list in chronological order. */
    using KeyframeList = std::vector<std::shared_ptr<Keyframe>>;

    /** @brief Runtime equivalent of an animation-content clip. */
    class AnimationClip : public System::Object
    {
    public:
        /**
         * @brief Constructs an animation clip.
         * @param duration Total clip duration.
         * @param keyframes All bone keyframes sorted by time.
         */
        AnimationClip(System::TimeSpan duration, KeyframeList keyframes);

        /** @brief Constructs an empty clip for the XNB deserializer. */
        AnimationClip() = default;

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `CpuSkinningDataTypes.AnimationClip`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets the total clip duration.
         * @return Clip duration.
         */
        [[nodiscard]] System::TimeSpan getDurationProperty() const { return duration; }

        /**
         * @brief Gets all keyframes for all bones, sorted by time.
         * @return Chronological keyframe list.
         */
        [[nodiscard]] const KeyframeList& getKeyframesProperty() const { return keyframes; }

    private:
        friend struct CpuSkinningContentReaderRegistrationEXT;

        System::TimeSpan duration;
        KeyframeList keyframes;
    };
}
