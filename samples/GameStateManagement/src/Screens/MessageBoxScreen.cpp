// SPDX-License-Identifier: MS-PL

#include "Screens/MessageBoxScreen.hpp"

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "ScreenManager/InputState.hpp"
#include "ScreenManager/ScreenManager.hpp"

namespace GameStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using Microsoft::Xna::Framework::Graphics::Texture2D;

    MessageBoxScreen::MessageBoxScreen(const std::string& message)
        : MessageBoxScreen(message, true) {}

    MessageBoxScreen::MessageBoxScreen(const std::string& message, bool includeUsageText)
    {
        static const std::string usageText =
            "\nA button, Space, Enter = ok\nB button, Esc = cancel";
        message_ = includeUsageText ? message + usageText : message;
        setIsPopupProperty(true);
        setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(0.2));
        setTransitionOffTimeProperty(System::TimeSpan::FromSeconds(0.2));
    }

    void MessageBoxScreen::LoadContent()
    {
        gradientTexture_.emplace(getScreenManagerProperty().getGameProperty()
            .getContentProperty().Load<Texture2D>("gradient"));
    }

    void MessageBoxScreen::HandleInput(InputState& input)
    {
        PlayerIndex playerIndex;
        if (input.IsMenuSelect(getControllingPlayerProperty(), playerIndex))
        {
            Accepted.Raise(this, PlayerIndexEventArgs(playerIndex));
            ExitScreen();
        }
        else if (input.IsMenuCancel(getControllingPlayerProperty(), playerIndex))
        {
            Cancelled.Raise(this, PlayerIndexEventArgs(playerIndex));
            ExitScreen();
        }
    }

    void MessageBoxScreen::Draw(const GameTime&)
    {
        auto& manager = getScreenManagerProperty();
        manager.FadeBackBufferToBlack(getTransitionAlphaProperty() * 2.0f / 3.0f);
        auto& font = manager.getFontProperty();
        auto& viewport = manager.getGraphicsDeviceProperty().getViewportProperty();
        const Vector2 viewportSize(static_cast<float>(viewport.getWidthProperty()),
                                   static_cast<float>(viewport.getHeightProperty()));
        const Vector2 textSize = font.MeasureString(message_);
        const Vector2 textPosition = (viewportSize - textSize) / 2.0f;
        constexpr int hPad = 32;
        constexpr int vPad = 16;
        const Rectangle backgroundRectangle(static_cast<int>(textPosition.X) - hPad,
            static_cast<int>(textPosition.Y) - vPad, static_cast<int>(textSize.X) + hPad * 2,
            static_cast<int>(textSize.Y) + vPad * 2);
        const Color color = Color::White * getTransitionAlphaProperty();
        auto& spriteBatch = manager.getSpriteBatchProperty();
        spriteBatch.Begin();
        spriteBatch.Draw(*gradientTexture_, backgroundRectangle, color);
        spriteBatch.DrawString(font, message_, textPosition, color);
        spriteBatch.End();
    }

    const std::string& MessageBoxScreen::GetTypeName() const
    {
        static const std::string name = "GameStateManagement.MessageBoxScreen";
        return name;
    }
}
