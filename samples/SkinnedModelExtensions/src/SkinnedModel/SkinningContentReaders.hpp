// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"

namespace SkinnedModel
{
    /** @brief Registers the AOT equivalents of XNA's reflected sample-owned XNB readers. */
    struct SkinningContentReaderRegistrationEXT
    {
        /** @brief Registers the animation, bone-index and skinned-sphere reader graph. */
        CNAEXT static void RegisterEXT();
    };
}
