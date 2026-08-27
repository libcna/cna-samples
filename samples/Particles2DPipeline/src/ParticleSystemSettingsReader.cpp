// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// Reads the ParticleSystemSettings the XNA content pipeline wrote. See the header.
//-----------------------------------------------------------------------------

#include "ParticleSystemSettingsReader.hpp"

#include <string>
#include <utility>

#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"

namespace Particles2DPipelineSample
{
    using Microsoft::Xna::Framework::Content::ContentReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;
    using ParticlesSettings::AccelerationMode;
    using ParticlesSettings::ParticleSystemSettings;
    using Microsoft::Xna::Framework::Graphics::Blend;

    ParticleSystemSettingsReader::ParticleSystemSettingsReader()
        : ContentTypeReader<ParticleSystemSettings>("ParticlesSettings.ParticleSystemSettings")
    {
    }

    ParticleSystemSettings ParticleSystemSettingsReader::Read(
        ContentReader& input, std::optional<ParticleSystemSettings> existingInstance)
    {
        ParticleSystemSettings settings = existingInstance.value_or(ParticleSystemSettings{});

        // Declaration order, exactly as IntermediateSerializer wrote it. Value types are inline;
        // TextureFilename is a reference type, so ReadObject consumes its type-reader index first.
        settings.MinNumParticles = input.ReadInt32();
        settings.MaxNumParticles = input.ReadInt32();
        settings.TextureFilename = input.ReadObject<std::string>();
        settings.MinDirectionAngle = input.ReadSingle();
        settings.MaxDirectionAngle = input.ReadSingle();
        settings.MinInitialSpeed = input.ReadSingle();
        settings.MaxInitialSpeed = input.ReadSingle();
        settings.AccelerationMode = (AccelerationMode)input.ReadInt32();
        settings.EndVelocity = input.ReadSingle();
        settings.MinAccelerationScale = input.ReadSingle();
        settings.MaxAccelerationScale = input.ReadSingle();
        settings.MinAccelerationVector = input.ReadVector2();
        settings.MaxAccelerationVector = input.ReadVector2();
        settings.EmitterVelocitySensitivity = input.ReadSingle();
        settings.MinRotationSpeed = input.ReadSingle();
        settings.MaxRotationSpeed = input.ReadSingle();
        settings.MinLifetime = input.ReadSingle();
        settings.MaxLifetime = input.ReadSingle();
        settings.MinSize = input.ReadSingle();
        settings.MaxSize = input.ReadSingle();
        settings.Gravity = input.ReadVector2();
        settings.SourceBlend = (Blend)input.ReadInt32();
        settings.DestinationBlend = (Blend)input.ReadInt32();

        return settings;
    }

    namespace
    {
        /// A .xnb's type-reader table must resolve in full before any object is read, so the two
        /// EnumReaders the settings file names need to exist even though the reflective payload
        /// writes each enum inline as an Int32 and this reader takes them that way.
        template <typename TEnum>
        class EnumAsInt32Reader
            : public Microsoft::Xna::Framework::Content::ContentTypeReader<TEnum>
        {
        public:
            explicit EnumAsInt32Reader(std::string targetTypeName)
                : Microsoft::Xna::Framework::Content::ContentTypeReader<TEnum>(
                      std::move(targetTypeName))
            {
            }

        protected:
            TEnum Read(ContentReader& input, std::optional<TEnum>) override
            {
                return (TEnum)input.ReadInt32();
            }
        };
    }

    void ParticleSystemSettingsReader::Register()
    {
        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.EnumReader`1"
            "[[ParticlesSettings.AccelerationMode]]",
            [] {
                return std::make_unique<EnumAsInt32Reader<AccelerationMode>>(
                    "ParticlesSettings.AccelerationMode");
            });

        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.EnumReader`1"
            "[[Microsoft.Xna.Framework.Graphics.Blend]]",
            [] {
                return std::make_unique<EnumAsInt32Reader<Blend>>(
                    "Microsoft.Xna.Framework.Graphics.Blend");
            });

        // The .xnb's table carries the assembly-qualified name; CNA normalizes it to this
        // canonical form (NormalizeXnbTypeReaderName), which is what the registry is keyed by.
        ContentTypeReaderManager::AddTypeCreator(
            "Microsoft.Xna.Framework.Content.ReflectiveReader`1"
            "[[ParticlesSettings.ParticleSystemSettings]]",
            [] { return std::make_unique<ParticleSystemSettingsReader>(); });
    }
}
