// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Content/ContentLoadException.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "System/Globalization/CultureInfo.hpp"

namespace Localization
{
    /** @brief Demonstrates localized strings and culture-specific content. */
    class LocalizationGame final : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Creates the localization sample game. */
        LocalizationGame();

        /** @brief Gets the fully qualified runtime type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads the sprite font and localized flag. */
        void LoadContent() override;

        /** @brief Handles the input used to exit the sample. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /** @brief Draws the localized strings and flag. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        template<typename T>
        T LoadLocalizedAsset(const std::string& assetName)
        {
            using System::Globalization::CultureInfo;
            using Microsoft::Xna::Framework::Content::ContentLoadException;

            const CultureInfo& culture = CultureInfo::getCurrentCultureProperty();
            const std::array<std::string, 2> cultureNames{
                culture.getNameProperty(),
                culture.getTwoLetterISOLanguageNameProperty(),
            };

            for (const std::string& cultureName : cultureNames)
            {
                try
                {
                    return getContentProperty().Load<T>(assetName + '.' + cultureName);
                }
                catch (const ContentLoadException&)
                {
                }
            }

            return getContentProperty().Load<T>(assetName);
        }

        void HandleInput();

        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch_;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> currentFlag_;
    };
}
