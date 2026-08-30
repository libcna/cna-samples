// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Keyframe.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "System/Object.hpp"
#include "System/TimeSpan.hpp"

namespace SkinnedModel
{
    using Microsoft::Xna::Framework::Matrix;

    /** @brief Grants the AOT content-reader registration access to serialized properties. */
    struct SkinningContentReaderRegistrationEXT;

    /** @brief Describes the position of one bone at one point in time. */
    class Keyframe : public System::Object
    {
    public:
        /**
         * @brief Constructs a keyframe.
         * @param bone Index of the target bone.
         * @param time Time offset from the start of the animation.
         * @param transform Bone transform at this keyframe.
         */
        Keyframe(int bone, System::TimeSpan time, const Matrix& transform);

        /** @brief Constructs an empty keyframe for the XNB deserializer. */
        Keyframe() = default;

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `SkinnedModel.Keyframe`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets the target bone index.
         * @return Target bone index.
         */
        [[nodiscard]] int getBoneProperty() const { return bone; }

        /**
         * @brief Gets the time offset.
         * @return Time offset from the start of the animation.
         */
        [[nodiscard]] System::TimeSpan getTimeProperty() const { return time; }

        /**
         * @brief Gets the bone transform.
         * @return Bone transform at this keyframe.
         */
        [[nodiscard]] const Matrix& getTransformProperty() const { return transform; }

    private:
        friend struct SkinningContentReaderRegistrationEXT;

        int bone = 0;
        System::TimeSpan time;
        Matrix transform;
    };
}
