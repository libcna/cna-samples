// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// IMessageDisplay.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>

#include "Microsoft/Xna/Framework/GameTime.hpp"

namespace NetworkStateManagement
{
    /**
     * @brief Interface used to display notification messages when interesting events occur, for
     * instance when gamers join or leave the network session. This interface is registered as a
     * service, so any piece of code wanting to display a message can look it up from
     * Game.Services, without needing to worry about how the message display is implemented.
     */
    class IMessageDisplay
    {
    public:
        /** @brief Releases the interface. */
        virtual ~IMessageDisplay() = default;

        /**
         * @brief Updates the notifications (the original's IUpdateable half).
         *
         * @param gameTime Timing information.
         */
        virtual void Update(Microsoft::Xna::Framework::GameTime& gameTime) = 0;

        /**
         * @brief Draws the notifications (the original's IDrawable half).
         *
         * @param gameTime Timing information.
         */
        virtual void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) = 0;

        /**
         * @brief Shows a formatted message; `{0}`, `{1}`... take the parameters in order.
         *
         * @param message Format string.
         * @param parameters Values for the placeholders.
         */
        virtual void ShowMessage(const std::string& message, const std::vector<std::string>& parameters) = 0;
    };
}
