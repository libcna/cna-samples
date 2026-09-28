// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NetworkBusyScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Networking/OperationCompletedEventArgs.hpp"
#include "ScreenManager/GameScreen.hpp"
#include "System/EventHandler.hpp"
#include "System/IAsyncResult.hpp"

namespace NetworkStateManagement
{
    /**
     * @brief When an asynchronous network operation (for instance searching for or joining a
     * session) is in progress, we want to display some sort of busy indicator to let the user know
     * the game hasn't just locked up. We also want to make sure they can't pick some other menu
     * option during this time, as that could cause problems. This screen gets pushed on top of the
     * previous menu, displays an animation, and raises OperationCompleted once the operation ends.
     */
    class NetworkBusyScreen final : public GameScreen
    {
    public:
        /** @brief Event handler for when the asynchronous operation has completed. */
        System::EventHandler<OperationCompletedEventArgs> OperationCompleted;

        /**
         * @brief Constructs a network busy screen for the specified asynchronous operation.
         *
         * @param asyncResult The operation to wait for.
         */
        explicit NetworkBusyScreen(System::IAsyncResult* asyncResult);

        /** @brief Loads graphics content for this screen. */
        void LoadContent() override;
        /**
         * @brief Updates the network busy screen.
         *
         * @param gameTime Timing information.
         * @param otherScreenHasFocus Whether another screen has focus.
         * @param coveredByOtherScreen Whether another screen covers this one.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime, bool otherScreenHasFocus,
                    bool coveredByOtherScreen) override;
        /** @brief Draws the network busy screen. @param gameTime Timing information. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;
        /** @brief Returns the managed type name. @return The type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        System::IAsyncResult* asyncResult;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> gradientTexture;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> catTexture;
    };
}
