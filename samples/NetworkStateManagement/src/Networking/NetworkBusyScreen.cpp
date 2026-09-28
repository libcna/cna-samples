// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NetworkBusyScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Networking/NetworkBusyScreen.hpp"

#include <algorithm>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Resources.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace NetworkStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    NetworkBusyScreen::NetworkBusyScreen(System::IAsyncResult* asyncResult)
        : asyncResult(asyncResult)
    {
        setIsPopupProperty(true);
        setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(0.1));
        setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.2));
    }

    void NetworkBusyScreen::LoadContent()
    {
        auto& content = getScreenManagerProperty().getGameProperty().getContentProperty();
        gradientTexture.emplace(content.Load<Texture2D>("gradient"));
        catTexture.emplace(content.Load<Texture2D>("cat"));
    }

    void NetworkBusyScreen::Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen)
    {
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

        // Has our asynchronous operation completed?
        if ((asyncResult != nullptr) && asyncResult->getIsCompletedProperty())
        {
            // If so, raise the OperationCompleted event.
            OperationCompleted.Raise(this, OperationCompletedEventArgs(asyncResult));

            ExitScreen();

            asyncResult = nullptr;
        }
    }

    void NetworkBusyScreen::Draw(const GameTime& gameTime)
    {
        auto& manager = getScreenManagerProperty();
        auto& spriteBatch = manager.getSpriteBatchProperty();
        auto& font = manager.getFontProperty();

        const std::string& message = Resources::NetworkBusy;

        constexpr int hPad = 32;
        constexpr int vPad = 16;

        // Center the message text in the viewport.
        const Viewport viewport = manager.getGraphicsDeviceProperty().getViewportProperty();
        const Vector2 viewportSize(static_cast<float>(viewport.getWidthProperty()), static_cast<float>(viewport.getHeightProperty()));
        Vector2 textSize = font.MeasureString(message);

        // Add enough room to spin a cat.
        const Vector2 catSize(static_cast<float>(catTexture->getWidthProperty()));

        textSize.X = std::max(textSize.X, catSize.X);
        textSize.Y += catSize.Y + vPad;

        const Vector2 textPosition = (viewportSize - textSize) / 2.0f;

        // The background includes a border somewhat larger than the text itself.
        const Rectangle backgroundRectangle(static_cast<int>(textPosition.X) - hPad,
                                            static_cast<int>(textPosition.Y) - vPad,
                                            static_cast<int>(textSize.X) + hPad * 2,
                                            static_cast<int>(textSize.Y) + vPad * 2);

        // Fade the popup alpha during transitions.
        const Color color = Color::White * getTransitionAlphaProperty();

        spriteBatch.Begin();

        // Draw the background rectangle.
        spriteBatch.Draw(*gradientTexture, backgroundRectangle, color);

        // Draw the message box text.
        spriteBatch.DrawString(font, message, textPosition, color);

        // Draw the spinning cat progress indicator.
        const float catRotation = static_cast<float>(gameTime.getTotalGameTimeProperty().getTotalSecondsProperty()) * 3.0f;

        const Vector2 catPosition(textPosition.X + textSize.X / 2.0f, textPosition.Y + textSize.Y - catSize.Y / 2.0f);

        spriteBatch.Draw(*catTexture, catPosition, std::nullopt, color, catRotation, catSize / 2.0f, 1.0f,
                         SpriteEffects::None, 0.0f);

        spriteBatch.End();
    }

    const std::string& NetworkBusyScreen::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.NetworkBusyScreen";
        return name;
    }
}
