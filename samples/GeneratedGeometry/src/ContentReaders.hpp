// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <optional>

#include "CNA/CNAHelper.hpp"

#include "Sky.hpp"

#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Model.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace GeneratedGeometry::ContentReaders
{
    using Microsoft::Xna::Framework::Content::ContentReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;
    using Microsoft::Xna::Framework::Graphics::Model;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /** @brief Canonical XNB reader name emitted for the original reflective Sky reader. */
    inline constexpr const char* SkyReaderName =
        "Microsoft.Xna.Framework.Content.ReflectiveReader`1[[GeneratedGeometry.Sky]]";

    /** @brief AOT equivalent of the original XNA reflective reader for Sky. */
    class SkyReader : public ContentTypeReader<Sky>
    {
    public:
        /** @brief Constructs the reader for the GeneratedGeometry.Sky runtime type. */
        SkyReader()
            : ContentTypeReader<Sky>("GeneratedGeometry.Sky")
        {
        }

    protected:
        Sky Read(ContentReader& input, std::optional<Sky>) override
        {
            Sky sky;
            sky.Model = input.ReadObject<Model>();
            sky.Texture = input.ReadObject<Texture2D>();
            return sky;
        }
    };

    /** @brief Registers the closed custom reader used by sky.xnb. */
    CNAEXT inline void Register()
    {
        ContentTypeReaderManager::AddTypeCreator(
            SkyReaderName,
            [] { return std::make_unique<SkyReader>(); });
    }
}
