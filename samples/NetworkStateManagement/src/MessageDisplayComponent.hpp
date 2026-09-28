// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// MessageDisplayComponent.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "IMessageDisplay.hpp"
#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "System/TimeSpan.hpp"

namespace NetworkStateManagement
{
    /**
     * @brief Component implements the IMessageDisplay interface. This is used to show notification
     * messages when interesting events occur, for instance when gamers join or leave the network
     * session.
     */
    class MessageDisplayComponent final : public Microsoft::Xna::Framework::DrawableGameComponent, public IMessageDisplay
    {
    public:
        /**
         * @brief Constructs a new message display component and registers it as the IMessageDisplay service.
         *
         * @param game The game.
         */
        explicit MessageDisplayComponent(Microsoft::Xna::Framework::Game& game);

        /** @brief Updates the message display component. @param gameTime Timing information. */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Draws the message display component. @param gameTime Timing information. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;
        /**
         * @brief Shows a new notification message.
         *
         * @param message Format string.
         * @param parameters Values for the placeholders.
         */
        void ShowMessage(const std::string& message, const std::vector<std::string>& parameters) override;

        /** @brief Returns the managed type name. @return The type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Load graphics content for the message display. */
        void LoadContent() override;

    private:
        // Helper class stores the position and text of a single notification message.
        struct NotificationMessage
        {
            std::string Text;
            float Position;
            System::TimeSpan Age;
        };

        std::optional<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font;

        // List of the currently visible notification messages.
        std::vector<NotificationMessage> messages;

        // Coordinates threadsafe access to the message list.
        std::mutex syncObject;
    };
}
