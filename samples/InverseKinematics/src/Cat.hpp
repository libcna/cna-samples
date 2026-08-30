// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Graphics/BasicEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexPositionTexture.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"

#include <array>

namespace InverseKinematicsSample
{
    using SharpRuntime::Single;

    /** @brief Entity that always faces the camera. */
    class Cat
    {
    public:
        /**
         * @brief Creates a billboard sprite that the IK chains attempt to reach.
         *
         * @param device Graphics device used to render the sprite.
         */
        explicit Cat(Microsoft::Xna::Framework::Graphics::GraphicsDevice& device);

        /**
         * @brief Gets the scale of the entity.
         *
         * @return Current scale.
         */
        [[nodiscard]] Single getScaleProperty() const;

        /**
         * @brief Sets the scale of the entity.
         *
         * @param value New scale.
         */
        void setScaleProperty(Single value);

        /**
         * @brief Gets the 3D position of the entity.
         *
         * @return Current position.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getPositionProperty() const;

        /**
         * @brief Sets the 3D position of the entity.
         *
         * @param value New position.
         */
        void setPositionProperty(const Microsoft::Xna::Framework::Vector3& value);

        /**
         * @brief Gets the orientation of the entity.
         *
         * @return Current up vector.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Vector3 getUpProperty() const;

        /**
         * @brief Sets the orientation of the entity.
         *
         * @param value New up vector.
         */
        void setUpProperty(const Microsoft::Xna::Framework::Vector3& value);

        /**
         * @brief Gets the texture used to display the entity.
         *
         * @return Current texture.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::Texture2D* getTextureProperty() const;

        /**
         * @brief Sets the texture used to display the entity.
         *
         * @param value New texture.
         */
        void setTextureProperty(Microsoft::Xna::Framework::Graphics::Texture2D* value);

        /**
         * @brief Draws the billboard sprite with transparency.
         *
         * @param cameraPosition Camera position in world space.
         * @param view View transform.
         * @param projection Projection transform.
         */
        void Draw(const Microsoft::Xna::Framework::Vector3& cameraPosition,
                  const Microsoft::Xna::Framework::Matrix& view,
                  const Microsoft::Xna::Framework::Matrix& projection);

        /**
         * @brief Draws a quadrilateral as part of the 3D world.
         *
         * @param texture Texture to draw.
         * @param textureRepeats Number of texture repetitions across the quad.
         * @param world World transform.
         * @param view View transform.
         * @param projection Projection transform.
         */
        void DrawQuad(Microsoft::Xna::Framework::Graphics::Texture2D* texture,
                      Single textureRepeats,
                      const Microsoft::Xna::Framework::Matrix& world,
                      const Microsoft::Xna::Framework::Matrix& view,
                      const Microsoft::Xna::Framework::Matrix& projection);

    private:
        Microsoft::Xna::Framework::Graphics::GraphicsDevice& graphicsDevice;
        Microsoft::Xna::Framework::Graphics::BasicEffect basicEffect;
        std::array<Microsoft::Xna::Framework::Graphics::VertexPositionTexture, 6> vertices{};
        Single scale = 0.0f;
        Microsoft::Xna::Framework::Vector3 position{};
        Microsoft::Xna::Framework::Vector3 up = Microsoft::Xna::Framework::Vector3::Up;
        Microsoft::Xna::Framework::Graphics::Texture2D* texture = nullptr;
    };
}
