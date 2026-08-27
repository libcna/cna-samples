// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Describes ParticleSystemSettings to CNA's reflective reader. See the header.
//-----------------------------------------------------------------------------

#include "ParticleSystemSettingsReader.hpp"

#include "Microsoft/Xna/Framework/Content/ReflectiveTypeReader.hpp"

#include "ParticleSystemSettings.hpp"

namespace Particles2DPipelineSample
{
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReaderBuilder;
    using ParticlesSettings::ParticleSystemSettings;

    void RegisterParticleSystemSettingsReader()
    {
        // Declaration order IS the wire order: XNA's IntermediateSerializer writes a type's public
        // fields in the order the type declares them, so this list must match
        // ParticleSystemSettings.hpp exactly, top to bottom.
        using S = ParticleSystemSettings;
        ReflectiveTypeReaderBuilder<S>("ParticlesSettings.ParticleSystemSettings")
            .Field(&S::MinNumParticles)
            .Field(&S::MaxNumParticles)
            .Field(&S::TextureFilename)
            .Field(&S::MinDirectionAngle)
            .Field(&S::MaxDirectionAngle)
            .Field(&S::MinInitialSpeed)
            .Field(&S::MaxInitialSpeed)
            .EnumField(&S::AccelerationMode, "ParticlesSettings.AccelerationMode")
            .Field(&S::EndVelocity)
            .Field(&S::MinAccelerationScale)
            .Field(&S::MaxAccelerationScale)
            .Field(&S::MinAccelerationVector)
            .Field(&S::MaxAccelerationVector)
            .Field(&S::EmitterVelocitySensitivity)
            .Field(&S::MinRotationSpeed)
            .Field(&S::MaxRotationSpeed)
            .Field(&S::MinLifetime)
            .Field(&S::MaxLifetime)
            .Field(&S::MinSize)
            .Field(&S::MaxSize)
            .Field(&S::Gravity)
            .EnumField(&S::SourceBlend, "Microsoft.Xna.Framework.Graphics.Blend")
            .EnumField(&S::DestinationBlend, "Microsoft.Xna.Framework.Graphics.Blend")
            .Register();
    }
}
