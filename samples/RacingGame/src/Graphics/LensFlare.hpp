// SPDX-License-Identifier: MS-PL

#pragma once

#include <array>
#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "System/IDisposable.hpp"

namespace Microsoft::Xna::Framework::Content
{
    class ContentManager;
}

namespace Microsoft::Xna::Framework::Graphics
{
    class GraphicsDevice;
}

namespace RacingGame::Graphics
{
    /** @brief Draws the original seven-texture Racing lens-flare composition. */
    class LensFlare : public System::IDisposable
    {
    public:
        /** @brief Original directional sun position used by the Racing game. */
        static Microsoft::Xna::Framework::Vector3 DefaultSunPos;
        /** @brief Original directional light position used by the Racing game. */
        static Microsoft::Xna::Framework::Vector3 DefaultLightPos;

        /**
         * @brief Rotates the original sun position around its X/Z plane.
         * @param rotation Rotation in radians.
         * @return Rotated directional sun position.
         */
        [[nodiscard]] static Microsoft::Xna::Framework::Vector3 RotateSun(
            float rotation);

        /**
         * @brief Loads all seven authentic lens-flare texture XNBs.
         * @param device Graphics device used by the additive sprite batch.
         * @param content Content manager rooted at the authentic XNA build.
         * @param lensOrigin Original directional sun origin.
         */
        LensFlare(
            Microsoft::Xna::Framework::Graphics::GraphicsDevice& device,
            Microsoft::Xna::Framework::Content::ContentManager& content,
            Microsoft::Xna::Framework::Vector3 lensOrigin);
        /** @brief Releases the authentic flare textures through RAII. */
        ~LensFlare() override;

        /** @brief Releases the seven authentic flare texture handles. */
        void Dispose() override;

        /**
         * @brief Sets the shared directional sun origin.
         * @param value New directional sun origin.
         */
        static void setOrigin3DProperty(
            Microsoft::Xna::Framework::Vector3 value);
        /**
         * @brief Gets the shared directional sun origin.
         * @return Current directional sun origin.
         */
        [[nodiscard]] static Microsoft::Xna::Framework::Vector3
        getOrigin3DProperty();

        /**
         * @brief Renders the source-authored additive flare sprites.
         *
         * The original reads camera matrices and tunnel state through BaseGame
         * globals; the C++ translation injects those same frame values.
         *
         * @param sunColor Tint applied to the authored flare colors.
         * @param view Current camera view matrix.
         * @param projection Current camera projection matrix.
         * @param enabled False while the current track segment is a tunnel.
         * @return Number of flare sprites submitted.
         */
        int Render(
            Microsoft::Xna::Framework::Color sunColor,
            const Microsoft::Xna::Framework::Matrix& view,
            const Microsoft::Xna::Framework::Matrix& projection,
            bool enabled);

        /** @brief Gets the latest submitted flare-sprite count. */
        [[nodiscard]] int getLastSubmissionCountProperty() const;
        /** @brief Gets the source-smoothed current sun intensity. */
        [[nodiscard]] float getSunIntensityProperty() const;

    private:
        static constexpr int NumberOfFlareTypes = 7;
        static Microsoft::Xna::Framework::Vector3 lensOrigin3D;

        Microsoft::Xna::Framework::Graphics::GraphicsDevice& device;
        Microsoft::Xna::Framework::Graphics::SpriteBatch spriteBatch;
        std::array<
            std::optional<Microsoft::Xna::Framework::Graphics::Texture2D>,
            NumberOfFlareTypes> flareTextures;
        float sunIntensity = 0.0f;
        int lastSubmissionCount = 0;
        bool isDisposed = false;
    };
}
