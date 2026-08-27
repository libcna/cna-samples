// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Describes ParticleSettings to CNA's reflective reader.
//
// Not a file of the original sample: XNA's content pipeline reflects over the type at load time
// and CNA cannot, so the game declares its own member list once and CNA builds the reader.
//-----------------------------------------------------------------------------
#pragma once

namespace Particle3DSample
{
    /**
     * @brief Registers the reader for `Particle3DSample.ParticleSettings`.
     *
     * Call once, before any settings asset is loaded.
     */
    void RegisterParticleSettingsReader();
}
