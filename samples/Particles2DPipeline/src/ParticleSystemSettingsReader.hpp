// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Describes ParticleSystemSettings to CNA's reflective reader.
//
// Not a file of the original sample: XNA needs no reader here, because its content pipeline
// compiles the settings XML through an implicit ReflectiveReader<T> that walks the type's fields
// with .NET reflection at load time. CNA has no such reflection, so the game declares its own
// field list once and CNA builds the reader from it.
//-----------------------------------------------------------------------------
#pragma once

namespace Particles2DPipelineSample
{
    /**
     * @brief Registers the reader for `ParticlesSettings.ParticleSystemSettings`.
     *
     * Call once, before any settings asset is loaded. Also registers the two `EnumReader`s the
     * `.xnb`'s type-reader table names.
     */
    void RegisterParticleSystemSettingsReader();
}
