// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Reads the ParticleSystemSettings the XNA content pipeline wrote.
//
// Not a file of the original sample: XNA needs no reader here, because its content pipeline
// compiles the settings XML through an implicit ReflectiveReader<T> that walks the type's fields
// with .NET reflection at load time. CNA has no such reflection and says so
// (docs/xnb-content-pipeline-support.md, XNB-42A), offering AddTypeCreator() instead -- so the
// GAME supplies the knowledge reflection would have provided, which it has anyway: its own type.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"

#include "ParticleSystemSettings.hpp"

namespace Particles2DPipelineSample
{
    /**
     * @brief Reads a `ParticlesSettings.ParticleSystemSettings` out of a pipeline-built `.xnb`.
     *
     * The reflective payload is not a mystery format: value-type fields are written inline in
     * declaration order, and a reference-type field is preceded by the 1-based index of its own
     * type reader. This reads exactly that, in the order `ParticleSystemSettings` declares.
     */
    class ParticleSystemSettingsReader
        : public Microsoft::Xna::Framework::Content::ContentTypeReader<
              ParticlesSettings::ParticleSystemSettings>
    {
    public:
        /** @brief Constructs the reader, naming the target type as the `.xnb` does. */
        ParticleSystemSettingsReader();

        /** @brief Registers this reader for the canonical reflective reader name. */
        static void Register();

    protected:
        /**
         * @brief Reads one settings object.
         * @param input            The reader positioned at the object's first field.
         * @param existingInstance An instance to deserialize into, when the caller supplied one.
         * @return The populated settings.
         */
        ParticlesSettings::ParticleSystemSettings Read(
            Microsoft::Xna::Framework::Content::ContentReader& input,
            std::optional<ParticlesSettings::ParticleSystemSettings> existingInstance) override;
    };
}
