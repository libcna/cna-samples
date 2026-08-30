// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CNAEXT -- the C++ counterpart of the reflection XNA performs for this game. Not upstream.
//-----------------------------------------------------------------------------
#pragma once

#include "CNA/CNAHelper.hpp"

namespace CustomModelSample
{
    /**
     * @brief Registers the reflective readers named by the custom model `.xnb`.
     *
     * XNA reconstructs `CustomModel` and its private `ModelPart` class by reflection. C++ has no
     * equivalent runtime field discovery, so this registration declares the same two field lists
     * once and lets CNA read the unchanged XNA pipeline output.
     */
    struct CNAEXT CustomModelContentReaderRegistrationEXT
    {
        /** @brief Registers the root, model-part and closed-list readers. */
        static void RegisterEXT();
    };
}
