// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"

namespace NinjAcademy {

/**
 * @brief Registers the reflective content readers used by the original NinjAcademy processors.
 */
struct CNAEXT NinjAcademyContentReaderRegistrationEXT {
    /** @brief Registers every reader named by Animations.xnb and Configuration.xnb. */
    static void RegisterEXT();
};

} // namespace NinjAcademy
