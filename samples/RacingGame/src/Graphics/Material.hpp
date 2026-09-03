// SPDX-License-Identifier: MS-PL

#pragma once

#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class Effect;
    class Texture2D;
}

namespace RacingGame::Graphics
{
    /** @brief Immutable material values extracted from a processed Racing model effect. */
    class Material
    {
    public:
        /**
         * @brief Extracts the original processor-authored material from an effect.
         * @param effect Model-part effect containing the material values.
         */
        explicit Material(
            Microsoft::Xna::Framework::Graphics::Effect& effect);

        /** @brief Gets the diffuse texture, or null when the material has none. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D*
        getDiffuseTextureProperty() const;
        /** @brief Gets the normal texture, or null when the material has none. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D*
        getNormalTextureProperty() const;
        /** @brief Gets the material diffuse color. */
        [[nodiscard]] const Microsoft::Xna::Framework::Vector4&
        getDiffuseColorProperty() const;
        /** @brief Gets the material ambient color. */
        [[nodiscard]] const Microsoft::Xna::Framework::Vector4&
        getAmbientColorProperty() const;
        /** @brief Gets the material specular color. */
        [[nodiscard]] const Microsoft::Xna::Framework::Vector4&
        getSpecularColorProperty() const;
        /** @brief Gets the material specular-power value. */
        [[nodiscard]] float getSpecularPowerProperty() const;
        /** @brief Reports whether the original texture format carries interpolated alpha. */
        [[nodiscard]] bool getHasAlphaProperty() const;
        /**
         * @brief Compares the values used by the original material batching key.
         * @param other Material to compare.
         * @return True when both materials can share one batch.
         */
        [[nodiscard]] bool Equals(const Material& other) const;

    private:
        Microsoft::Xna::Framework::Graphics::Texture2D* diffuseTexture = nullptr;
        Microsoft::Xna::Framework::Graphics::Texture2D* normalTexture = nullptr;
        Microsoft::Xna::Framework::Vector4 diffuseColor;
        Microsoft::Xna::Framework::Vector4 ambientColor;
        Microsoft::Xna::Framework::Vector4 specularColor;
        float specularPower = 24.0f;
    };
}
