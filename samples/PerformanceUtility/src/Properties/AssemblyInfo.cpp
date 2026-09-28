// SPDX-License-Identifier: MS-PL
// Copyright (C) Microsoft Corporation. All rights reserved.

#include "CNA/AssemblyInfo.hpp"
#include "CNA/ProjectGraphicsProfile.hpp"

namespace
{
    const CNA::AssemblyTitleAttributeEXT assemblyTitle{"PerformanceUtility"};
#if defined(WINDOWS_PHONE)
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::Reach};
#else
    const CNA::ProjectGraphicsProfileEXT projectGraphicsProfile{
        Microsoft::Xna::Framework::Graphics::GraphicsProfile::HiDef};
#endif
}
