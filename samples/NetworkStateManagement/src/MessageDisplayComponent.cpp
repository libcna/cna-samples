// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// MessageDisplayComponent.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "MessageDisplayComponent.hpp"

#include <algorithm>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameServiceContainer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteEffects.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"

namespace NetworkStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    namespace
    {
        // Tweakable settings control how long each message is visible.
        const System::TimeSpan fadeInTime = System::TimeSpan::FromSeconds(0.25);
        const System::TimeSpan showTime = System::TimeSpan::FromSeconds(5);
        const System::TimeSpan fadeOutTime = System::TimeSpan::FromSeconds(0.5);

        // string.Format for the "{n}" placeholders this sample uses.
        std::string Format(std::string message, const std::vector<std::string>& parameters)
        {
            for (std::size_t i = 0; i < parameters.size(); ++i)
            {
                const std::string placeholder = "{" + std::to_string(i) + "}";
                for (std::size_t at = message.find(placeholder); at != std::string::npos;
                     at = message.find(placeholder, at + parameters[i].size()))
                {
                    message.replace(at, placeholder.size(), parameters[i]);
                }
            }
            return message;
        }
    }

    MessageDisplayComponent::MessageDisplayComponent(Game& game)
        : DrawableGameComponent(game)
    {
        // Register ourselves to implement the IMessageDisplay service.
        game.getServicesProperty().AddService<IMessageDisplay>(this);
    }

    void MessageDisplayComponent::LoadContent()
    {
        spriteBatch.emplace(getGraphicsDeviceProperty());
        font.emplace(getGameProperty().getContentProperty().Load<SpriteFont>("menufont"));
    }

    void MessageDisplayComponent::Update(GameTime& gameTime)
    {
        std::lock_guard lock(syncObject);

        std::size_t index = 0;
        float targetPosition = 0.0f;

        // Update each message in turn.
        while (index < messages.size())
        {
            NotificationMessage& message = messages[index];

            // Gradually slide the message toward its desired position.
            const float positionDelta = targetPosition - message.Position;
            const float velocity = static_cast<float>(gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty()) * 2.0f;
            message.Position += positionDelta * std::min(velocity, 1.0f);

            // Update the age of the message.
            message.Age = message.Age + gameTime.getElapsedGameTimeProperty();

            if (message.Age < showTime + fadeOutTime)
            {
                // This message is still alive.
                index++;

                // Any subsequent messages should be positioned below
                // this one, unless it has started to fade out.
                if (message.Age < showTime)
                    targetPosition++;
            }
            else
            {
                // This message is old, and should be removed.
                messages.erase(messages.begin() + static_cast<std::ptrdiff_t>(index));
            }
        }
    }

    void MessageDisplayComponent::Draw(const GameTime&)
    {
        std::lock_guard lock(syncObject);

        // Early out if there are no messages to display.
        if (messages.empty())
            return;

        Vector2 position(static_cast<float>(getGraphicsDeviceProperty().getViewportProperty().getWidthProperty() - 100), 0.0f);

        spriteBatch->Begin();

        // Draw each message in turn.
        for (const NotificationMessage& message : messages)
        {
            constexpr float scale = 0.75f;

            // Compute the alpha of this message.
            float alpha = 1.0f;

            if (message.Age < fadeInTime)
            {
                // Fading in.
                alpha = static_cast<float>(message.Age.getTotalSecondsProperty() / fadeInTime.getTotalSecondsProperty());
            }
            else if (message.Age > showTime)
            {
                // Fading out.
                const System::TimeSpan fadeOut = showTime + fadeOutTime - message.Age;
                alpha = static_cast<float>(fadeOut.getTotalSecondsProperty() / fadeOutTime.getTotalSecondsProperty());
            }

            // Compute the message position.
            position.Y = 80.0f + message.Position * static_cast<float>(font->getLineSpacingProperty()) * scale;

            // Compute an origin value to right align each message.
            Vector2 origin = font->MeasureString(message.Text);
            origin.Y = 0.0f;

            // Draw the message text, with a drop shadow.
            spriteBatch->DrawString(*font, message.Text, position + Vector2::One, Color::Black * alpha, 0.0f,
                                    origin, scale, SpriteEffects::None, 0.0f);
            spriteBatch->DrawString(*font, message.Text, position, Color::White * alpha, 0.0f,
                                    origin, scale, SpriteEffects::None, 0.0f);
        }

        spriteBatch->End();
    }

    void MessageDisplayComponent::ShowMessage(const std::string& message, const std::vector<std::string>& parameters)
    {
        const std::string formattedMessage = Format(message, parameters);

        std::lock_guard lock(syncObject);

        const float startPosition = static_cast<float>(messages.size());
        messages.push_back(NotificationMessage{formattedMessage, startPosition, System::TimeSpan::Zero});
    }

    const std::string& MessageDisplayComponent::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.MessageDisplayComponent";
        return name;
    }
}
