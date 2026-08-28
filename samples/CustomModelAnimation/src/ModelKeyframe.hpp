// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// ModelKeyframe.cs
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

namespace CustomModelAnimation
{
    using Microsoft::Xna::Framework::Matrix;

    /** @brief Grants the content-reader registration access to the deserializer-written fields. */
    struct ModelContentReaderRegistrationEXT;

    /**
     * @brief Describes the position of a single bone at a single point in time.
     *
     * Derives from `System::Object` because the `.xnb` dispatches to this type through a list, and
     * the port reads it as a `std::shared_ptr` for the same reason the C# holds a reference.
     */
    class ModelKeyframe : public System::Object
    {
    public:
        /**
         * @brief Constructs a new ModelKeyframe object.
         *
         * @param bone The index of the target bone animated by this keyframe.
         * @param time The time offset from the start of the animation to this keyframe.
         * @param transform The bone transform for this keyframe.
         */
        ModelKeyframe(int bone, System::TimeSpan time, const Matrix& transform);

        /**
         * @brief Constructs an empty keyframe for the XNB deserializer to fill in.
         *
         * The C# marks this constructor private and lets the deserializer at it by reflection.
         * CNA's reflective reader constructs the object with `std::make_shared`, from outside the
         * class, so it has to be public here.
         */
        ModelKeyframe() = default;

        /**
         * @brief Returns the fully qualified logical type name of this object.
         * @return "CustomModelAnimation.ModelKeyframe".
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

        /**
         * @brief Gets the index of the target bone that is animated by this keyframe.
         * @return The bone index.
         */
        [[nodiscard]] int getBoneProperty() const { return bone; }

        /**
         * @brief Gets the time offset from the start of the animation to this keyframe.
         * @return The keyframe's time offset.
         */
        [[nodiscard]] System::TimeSpan getTimeProperty() const { return time; }

        /**
         * @brief Gets the bone transform for this keyframe.
         * @return The transform.
         */
        [[nodiscard]] const Matrix& getTransformProperty() const { return transform; }

    private:
        friend struct ModelContentReaderRegistrationEXT;

        // Written only by the constructor and by the XNB deserializer, which is what the C#
        // `{ get; private set; }` plus `[ContentSerializer]` says.
        int bone = 0;
        System::TimeSpan time;
        Matrix transform;
    };
}
