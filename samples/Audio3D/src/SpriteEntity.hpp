// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// SpriteEntity.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "AudioManager.hpp"
#include "IAudioEmitter.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Audio3D
{
    class QuadDrawer;

    /** @brief Base for billboard entities that can emit 3D sounds. */
    class SpriteEntity : public IAudioEmitter
    {
    public:
        /** @brief Destroys the sprite entity. */
        ~SpriteEntity() override = default;

        /**
         * @brief Gets the entity position.
         * @return The current world-space position.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getPositionProperty() const override;

        /**
         * @brief Sets the entity position.
         * @param value New world-space position.
         */
        void setPositionProperty(const Microsoft::Xna::Framework::Vector3& value);

        /**
         * @brief Gets the direction the entity is facing.
         * @return The current forward vector.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getForwardProperty() const override;

        /**
         * @brief Sets the direction the entity is facing.
         * @param value New forward vector.
         */
        void setForwardProperty(const Microsoft::Xna::Framework::Vector3& value);

        /**
         * @brief Gets the entity orientation.
         * @return The current up vector.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getUpProperty() const override;

        /**
         * @brief Sets the entity orientation.
         * @param value New up vector.
         */
        void setUpProperty(const Microsoft::Xna::Framework::Vector3& value);

        /**
         * @brief Gets how fast the entity is moving.
         * @return The current velocity.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getVelocityProperty() const override;

        /**
         * @brief Gets the texture used to display the entity.
         * @return The current texture, or null before content is loaded.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D* getTextureProperty() const;

        /**
         * @brief Sets the texture used to display the entity.
         * @param value New texture.
         */
        void setTextureProperty(Microsoft::Xna::Framework::Graphics::Texture2D* value);

        /**
         * @brief Updates the entity and allows it to play sounds.
         * @param gameTime Timing information for the current frame.
         * @param audioManager Manager used to play spatial sounds.
         */
        virtual void Update(
            Microsoft::Xna::Framework::GameTime& gameTime,
            AudioManager& audioManager) = 0;

        /**
         * @brief Draws the entity as a billboard sprite.
         * @param quadDrawer Helper used to draw the sprite quad.
         * @param cameraPosition Current camera position.
         * @param view View transform.
         * @param projection Projection transform.
         */
        void Draw(
            QuadDrawer& quadDrawer,
            const Microsoft::Xna::Framework::Vector3& cameraPosition,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection);

    protected:
        /**
         * @brief Sets how fast the entity is moving.
         * @param value New velocity.
         */
        void setVelocityProperty(const Microsoft::Xna::Framework::Vector3& value);

    private:
        Microsoft::Xna::Framework::Vector3 position;
        Microsoft::Xna::Framework::Vector3 forward;
        Microsoft::Xna::Framework::Vector3 up;
        Microsoft::Xna::Framework::Vector3 velocity;
        Microsoft::Xna::Framework::Graphics::Texture2D* texture = nullptr;
    };
}
