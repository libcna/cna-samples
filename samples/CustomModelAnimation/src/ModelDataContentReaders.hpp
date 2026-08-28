// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// CNAEXT -- the C++ counterpart of the reflection XNA does for the game here. Not upstream.
//-----------------------------------------------------------------------------
#pragma once

#include "CNA/CNAHelper.hpp"

namespace CustomModelAnimation
{
    /**
     * @brief Registers the readers for the types this sample's content processors serialize.
     *
     * XNA compiles a type that has no explicit `ContentTypeWriter` through an implicit
     * `ReflectiveReader<T>`, which walks the type's fields with .NET reflection at load time.
     * `AnimatedModelProcessor` and `SkinnedModelProcessor` do exactly that with `ModelData`,
     * `ModelAnimationClip` and `ModelKeyframe`, so the `.xnb`'s type-reader table names those
     * three plus the two closed generics over them.
     *
     * C++ has no reflection, so the game supplies the one thing reflection provided -- its own
     * field list -- and CNA builds the readers from it. This is the only call the original does
     * not have; see `diff.md`.
     */
    struct CNAEXT ModelContentReaderRegistrationEXT
    {
        /** @brief Registers every reader the two models' `.xnb` files name. */
        static void RegisterEXT();
    };
}
