// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.
#pragma once
#include "CNA/CNAHelper.hpp"
namespace CustomAvatarAnimation {
/** @brief AOT counterpart of XNA reflection for the exact sample data graph. */
class CustomAvatarAnimationContentReaderRegistrationEXT {
public:
    /** @brief Registers sample-owned readers and their closed generic lists. */
    CNAEXT static void RegisterEXT();
};
}
