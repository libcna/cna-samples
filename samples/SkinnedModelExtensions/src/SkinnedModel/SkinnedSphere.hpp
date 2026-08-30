// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SkinnedSphere.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>

#include "CNA/CNAHelper.hpp"
#include "System/Object.hpp"

namespace SkinnedModel
{
    /** @brief Describes a collision sphere attached to one skeleton bone. */
    class SkinnedSphere : public System::Object
    {
    public:
        /** @brief Name of the skeleton bone that owns this sphere. */
        std::string BoneName;
        /** @brief Sphere radius in model space. */
        float Radius = 0.0f;
        /** @brief Optional local X-axis displacement from the bone origin. */
        float Offset = 0.0f;

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `SkinnedModel.SkinnedSphere`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        friend struct SkinningContentReaderRegistrationEXT;
    };
}
