// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Describes ParticleSettings to CNA's reflective reader. See the header.
//-----------------------------------------------------------------------------

#include "ParticleSettingsReader.hpp"

#include <string>

#include "Microsoft/Xna/Framework/Content/ReflectiveTypeReader.hpp"
#include "System/ArgumentException.hpp"

#include "ParticleSettings.hpp"

namespace Particle3DSample
{
    using Microsoft::Xna::Framework::Content::ContentReader;
    using Microsoft::Xna::Framework::Content::ReflectiveTypeReaderBuilder;
    using Microsoft::Xna::Framework::Graphics::BlendState;

    void RegisterParticleSettingsReader()
    {
        using S = ParticleSettings;

        // WIRE ORDER, not the type's field order: IntermediateSerializer writes the serialized
        // PROPERTIES first and the public fields after, each group in declaration order. This
        // type's only property is the private [ContentSerializer] string that stands in for the
        // BlendState object, and it really does come out ahead of TextureName -- decoded from
        // FireSettings.xnb, which ends on its last byte with every value matching the XML.
        ReflectiveTypeReaderBuilder<S>("Particle3DSample.ParticleSettings")
            .Custom([](S& target, ContentReader& input) {
                // The original's BlendStateSerializationHelper setter, verbatim.
                const std::string value = input.ReadObject<std::string>();
                if (value == "AlphaBlend")
                    target.BlendState = BlendState::AlphaBlend;
                else if (value == "Additive")
                    target.BlendState = BlendState::Additive;
                else if (value == "NonPremultiplied")
                    target.BlendState = BlendState::NonPremultiplied;
                else
                    throw System::ArgumentException("Unknown blend state " + value);
            })
            .Field(&S::TextureName)
            .Field(&S::MaxParticles)
            .Field(&S::Duration)
            .Field(&S::DurationRandomness)
            .Field(&S::EmitterVelocitySensitivity)
            .Field(&S::MinHorizontalVelocity)
            .Field(&S::MaxHorizontalVelocity)
            .Field(&S::MinVerticalVelocity)
            .Field(&S::MaxVerticalVelocity)
            .Field(&S::Gravity)
            .Field(&S::EndVelocity)
            .Field(&S::MinColor)
            .Field(&S::MaxColor)
            .Field(&S::MinRotateSpeed)
            .Field(&S::MaxRotateSpeed)
            .Field(&S::MinStartSize)
            .Field(&S::MaxStartSize)
            .Field(&S::MinEndSize)
            .Field(&S::MaxEndSize)
            .Register();
    }
}
