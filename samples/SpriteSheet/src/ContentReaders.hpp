// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "SpriteSheet.hpp"

#include "CNA/Internal/Xnb/CollectionContentTypeReaders.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"

namespace SpriteSheetRuntime::ContentReaders
{
    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Content::ContentReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReader;
    using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /** @brief Canonical XNB reader name emitted for the reflective SpriteSheet reader. */
    inline constexpr const char* SpriteSheetReaderName =
        "Microsoft.Xna.Framework.Content.ReflectiveReader`1[[SpriteSheetRuntime.SpriteSheet]]";
    /** @brief Canonical XNB reader name emitted for the sprite-name dictionary. */
    inline constexpr const char* SpriteNameDictionaryReaderName =
        "Microsoft.Xna.Framework.Content.DictionaryReader`2[[System.String],[System.Int32]]";

    /** @brief AOT equivalent of the reflective XNA reader for SpriteSheet. */
    class SpriteSheetReader : public ContentTypeReader<SpriteSheet>
    {
    public:
        /** @brief Constructs the reader for the SpriteSheet runtime type. */
        SpriteSheetReader()
            : ContentTypeReader<SpriteSheet>("SpriteSheetRuntime.SpriteSheet")
        {
        }

    protected:
        SpriteSheet Read(ContentReader& input,
                         std::optional<SpriteSheet>) override
        {
            Texture2D texture = input.ReadObject<Texture2D>();
            std::vector<Rectangle> spriteRectangles =
                input.ReadObject<std::vector<Rectangle>>();
            std::unordered_map<std::string, int> spriteNames =
                input.ReadObject<std::unordered_map<std::string, int>>();

            return SpriteSheet(
                std::move(texture), std::move(spriteRectangles), std::move(spriteNames));
        }
    };

    /** @brief Registers the closed custom readers used by SpriteSheet.xnb. */
    inline void Register()
    {
        ContentTypeReaderManager::AddTypeCreator(
            SpriteSheetReaderName,
            [] { return std::make_unique<SpriteSheetReader>(); });
        ContentTypeReaderManager::AddTypeCreator(
            SpriteNameDictionaryReaderName,
            [] {
                return std::make_unique<CNA::Internal::Xnb::DictionaryReader<
                    std::string, int>>(
                        "System.Collections.Generic.Dictionary`2[[System.String],[System.Int32]]",
                        "Microsoft.Xna.Framework.Content.StringReader",
                        "Microsoft.Xna.Framework.Content.Int32Reader");
            });
    }
}
