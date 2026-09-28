// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// GroundEffect.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>

#include "Microsoft/Xna/Framework/Graphics/Effect.hpp"
#include "Microsoft/Xna/Framework/Graphics/EffectParameter.hpp"
#include "Microsoft/Xna/Framework/Graphics/IEffectMatrices.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

namespace AvatarShadows
{
    /**
     * @brief Provides a basic wrapper on top of an Effect to expose our parameters like the built
     * in effects.
     */
    class GroundEffect final : public Microsoft::Xna::Framework::Graphics::IEffectMatrices
    {
    public:
        /**
         * @brief Initializes a new GroundEffect on top of the given Effect.
         *
         * @param effect The loaded ground effect.
         */
        explicit GroundEffect(std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> effect);

        /**
         * @brief Gets the underlying Effect.
         *
         * @return The wrapped effect.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Effect& getBaseEffectProperty() const;

        /** @brief Gets the World matrix parameter. @return World matrix. */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix getWorldProperty() const override;
        /** @brief Sets the World matrix parameter. @param value World matrix. */
        void setWorldProperty(const Microsoft::Xna::Framework::Matrix& value) override;
        /** @brief Gets the View matrix parameter. @return View matrix. */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix getViewProperty() const override;
        /** @brief Sets the View matrix parameter. @param value View matrix. */
        void setViewProperty(const Microsoft::Xna::Framework::Matrix& value) override;
        /** @brief Gets the Projection matrix parameter. @return Projection matrix. */
        [[nodiscard]] Microsoft::Xna::Framework::Matrix getProjectionProperty() const override;
        /** @brief Sets the Projection matrix parameter. @param value Projection matrix. */
        void setProjectionProperty(const Microsoft::Xna::Framework::Matrix& value) override;

        /** @brief Gets the diffuse texture parameter. @return Diffuse texture. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D* getTextureProperty() const;
        /** @brief Sets the diffuse texture parameter. @param value Diffuse texture. */
        void setTextureProperty(Microsoft::Xna::Framework::Graphics::Texture2D* value);

        /** @brief Gets the shadow texture parameter. @return Shadow texture. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D* getShadowProperty() const;
        /** @brief Sets the shadow texture parameter. @param value Shadow texture. */
        void setShadowProperty(Microsoft::Xna::Framework::Graphics::Texture2D* value);

    private:
        std::shared_ptr<Microsoft::Xna::Framework::Graphics::Effect> baseEffect;
        Microsoft::Xna::Framework::Graphics::EffectParameter* world;
        Microsoft::Xna::Framework::Graphics::EffectParameter* view;
        Microsoft::Xna::Framework::Graphics::EffectParameter* projection;
        Microsoft::Xna::Framework::Graphics::EffectParameter* texture;
        Microsoft::Xna::Framework::Graphics::EffectParameter* shadow;
    };
}
