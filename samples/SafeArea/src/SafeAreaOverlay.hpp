// SPDX-License-Identifier: MS-PL

#pragma once

#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"

namespace SafeArea
{
    /** @brief Draws translucent borders outside the viewport title-safe area. */
    class SafeAreaOverlay : public Microsoft::Xna::Framework::DrawableGameComponent
    {
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::Texture2D> dummyTexture;

    public:
        /**
         * @brief Creates a safe-area overlay component.
         *
         * @param game Game that owns the component.
         */
        explicit SafeAreaOverlay(Microsoft::Xna::Framework::Game& game)
            : DrawableGameComponent(game)
        {
            setDrawOrderProperty(1000);
        }

        /**
         * @brief Returns the fully qualified runtime type name.
         *
         * @return Fully qualified .NET-compatible type name.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override
        {
            static const std::string name = "SafeArea.SafeAreaOverlay";
            return name;
        }

    protected:
        /** @brief Creates the sprite batch and one-pixel white overlay texture. */
        void LoadContent() override
        {
            using namespace Microsoft::Xna::Framework;
            using namespace Microsoft::Xna::Framework::Graphics;

            spriteBatch = std::make_unique<SpriteBatch>(getGraphicsDeviceProperty());
            dummyTexture = std::make_unique<Texture2D>(getGraphicsDeviceProperty(), 1, 1);

            Color white = Color::White;
            dummyTexture->SetData(&white, 1);
        }

    public:
        /**
         * @brief Draws the four translucent regions outside the title-safe area.
         *
         * @param gameTime Current timing snapshot.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override
        {
            using namespace Microsoft::Xna::Framework;
            using namespace Microsoft::Xna::Framework::Graphics;

            const Viewport viewport = getGraphicsDeviceProperty().getViewportProperty();
            const Rectangle safeArea = viewport.getTitleSafeAreaProperty();

            const int viewportRight =
                viewport.getXProperty() + viewport.getWidthProperty();
            const int viewportBottom =
                viewport.getYProperty() + viewport.getHeightProperty();

            const Rectangle leftBorder(
                viewport.getXProperty(),
                viewport.getYProperty(),
                safeArea.getLeftProperty() - viewport.getXProperty(),
                viewport.getHeightProperty());
            const Rectangle rightBorder(
                safeArea.getRightProperty(),
                viewport.getYProperty(),
                viewportRight - safeArea.getRightProperty(),
                viewport.getHeightProperty());
            const Rectangle topBorder(
                safeArea.getLeftProperty(),
                viewport.getYProperty(),
                safeArea.Width,
                safeArea.getTopProperty() - viewport.getYProperty());
            const Rectangle bottomBorder(
                safeArea.getLeftProperty(),
                safeArea.getBottomProperty(),
                safeArea.Width,
                viewportBottom - safeArea.getBottomProperty());

            const Color translucentRed = Color::Red * 0.5f;

            spriteBatch->Begin();
            spriteBatch->Draw(*dummyTexture, leftBorder, translucentRed);
            spriteBatch->Draw(*dummyTexture, rightBorder, translucentRed);
            spriteBatch->Draw(*dummyTexture, topBorder, translucentRed);
            spriteBatch->Draw(*dummyTexture, bottomBorder, translucentRed);
            spriteBatch->End();
        }
    };
}
