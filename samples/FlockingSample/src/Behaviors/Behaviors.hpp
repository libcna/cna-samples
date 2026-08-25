// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Behaviors.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>

#include "Behaviors/Behavior.hpp"
#include "System/Collections/Generic/List.hpp"

namespace Flocking
{
    /**
     * @brief A list of behaviors.
     *
     * Upstream this is `class Behaviors : List<Behavior>` and nothing more. The element is
     * an owning pointer here because C#'s list holds references to polymorphic behaviors
     * and C++ needs the ownership stated.
     */
    class Behaviors : public System::Collections::Generic::List<std::shared_ptr<Behavior>>
    {
    };
}
