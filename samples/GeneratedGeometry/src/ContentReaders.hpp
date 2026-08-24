// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <optional>
#include <utility>

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
            Model model = input.ReadObject<Model>();
            Texture2D texture = input.ReadObject<Texture2D>();
            return Sky(std::move(model), std::move(texture));
        }
    };

    /** @brief Registers the closed custom reader used by sky.xnb. */
    inline void Register()
    {
        ContentTypeReaderManager::AddTypeCreator(
            SkyReaderName,
            [] { return std::make_unique<SkyReader>(); });
    }
}
