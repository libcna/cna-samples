// SPDX-License-Identifier: MS-PL

#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/ArgumentOutOfRangeException.hpp"
#include "System/Collections/Generic/KeyNotFoundException.hpp"

namespace SpriteSheetRuntime
{
    namespace ContentReaders
    {
        class SpriteSheetReader;
    }

    using Microsoft::Xna::Framework::Rectangle;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    /**
     * @brief Contains many sprite images packed into one texture and their lookup metadata.
     */
    class SpriteSheet
    {
        friend class ContentReaders::SpriteSheetReader;

        Texture2D texture_;
        std::vector<Rectangle> spriteRectangles_;
        std::unordered_map<std::string, int> spriteNames_;

        SpriteSheet(Texture2D texture,
                    std::vector<Rectangle> spriteRectangles,
                    std::unordered_map<std::string, int> spriteNames)
            : texture_(std::move(texture))
            , spriteRectangles_(std::move(spriteRectangles))
            , spriteNames_(std::move(spriteNames))
        {
        }

    public:
        /** @brief Constructs an empty sprite sheet for content deserialization. */
        SpriteSheet() = default;

        /**
         * @brief Gets the single large texture used by this sprite sheet.
         *
         * @return The packed sprite texture.
         */
        [[nodiscard]] const Texture2D& getTextureProperty() const
        {
            return texture_;
        }

        /**
         * @brief Looks up the location of a named sprite within the packed texture.
         *
         * @param spriteName Original sprite filename without its extension.
         * @return The sprite's source rectangle.
         */
        [[nodiscard]] Rectangle SourceRectangle(const std::string& spriteName) const
        {
            const int spriteIndex = GetIndex(spriteName);
            return spriteRectangles_[static_cast<std::size_t>(spriteIndex)];
        }

        /**
         * @brief Looks up the location of a numbered sprite within the packed texture.
         *
         * @param spriteIndex Zero-based sprite index.
         * @return The sprite's source rectangle.
         */
        [[nodiscard]] Rectangle SourceRectangle(int spriteIndex) const
        {
            if (spriteIndex < 0 ||
                spriteIndex >= static_cast<int>(spriteRectangles_.size()))
            {
                throw System::ArgumentOutOfRangeException("spriteIndex");
            }

            return spriteRectangles_[static_cast<std::size_t>(spriteIndex)];
        }

        /**
         * @brief Looks up the numeric index of a named sprite.
         *
         * @param spriteName Original sprite filename without its extension.
         * @return The sprite's zero-based index.
         */
        [[nodiscard]] int GetIndex(const std::string& spriteName) const
        {
            const auto iterator = spriteNames_.find(spriteName);
            if (iterator == spriteNames_.end())
            {
                throw System::Collections::Generic::KeyNotFoundException(
                    "SpriteSheet does not contain a sprite named '" + spriteName + "'.");
            }

            return iterator->second;
        }
    };
}
