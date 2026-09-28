// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// LoadingScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "Screens/LoadingScreen.hpp"

#include <utility>

#include "IMessageDisplay.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameServiceContainer.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/Viewport.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionState.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Resources.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "System/Diagnostics/Stopwatch.hpp"
#include "System/Threading/EventResetMode.hpp"

namespace NetworkStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
    using Microsoft::Xna::Framework::Net::NetworkSession;
    using Microsoft::Xna::Framework::Net::NetworkSessionState;

    // The constructor is private: loading screens should be activated via the static Load method instead.
    LoadingScreen::LoadingScreen(ScreenManager& screenManager, bool loadingIsSlow,
                                 std::vector<std::shared_ptr<GameScreen>> screensToLoad)
        : loadingIsSlow_(loadingIsSlow), screensToLoad_(std::move(screensToLoad))
    {
        setTransitionOnTimeProperty(System::TimeSpan::FromSeconds(0.5));

        // If this is going to be a slow load operation, create a background
        // thread that will update the network session and draw the load screen
        // animation while the load is taking place.
        if (loadingIsSlow)
        {
            backgroundThreadExit = std::make_unique<System::Threading::EventWaitHandle>(
                false, System::Threading::EventResetMode::ManualReset);

            graphicsDevice = &screenManager.getGraphicsDeviceProperty();

            // Look up some services that will be used by the background thread.
            auto& services = screenManager.getGameProperty().getServicesProperty();

            networkSession = services.GetService<NetworkSession>();

            messageDisplay = services.GetService<IMessageDisplay>();
        }
    }

    LoadingScreen::~LoadingScreen()
    {
        if (backgroundThread && backgroundThread->joinable())
        {
            backgroundThreadExit->Set();
            backgroundThread->join();
        }
    }

    void LoadingScreen::Load(ScreenManager& screenManager, bool loadingIsSlow,
                             std::optional<PlayerIndex> controllingPlayer,
                             std::vector<std::shared_ptr<GameScreen>> screensToLoad)
    {
        // Tell all the current screens to transition off.
        for (const auto& screen : screenManager.GetScreens())
            screen->ExitScreen();

        // Create and activate the loading screen.
        std::shared_ptr<LoadingScreen> loadingScreen(new LoadingScreen(screenManager, loadingIsSlow, std::move(screensToLoad)));

        screenManager.AddScreen(std::move(loadingScreen), controllingPlayer);
    }

    void LoadingScreen::Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen)
    {
        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);

        // If all the previous screens have finished transitioning
        // off, it is time to actually perform the load.
        if (otherScreensAreGone_)
        {
            // Start up the background thread, which will update the network
            // session and draw the animation while we are loading.
            if (backgroundThreadExit)
            {
                loadStartTime = gameTime;
                backgroundThread.emplace([this] { BackgroundWorkerThread(); });
            }

            // Perform the load operation.
            auto& manager = getScreenManagerProperty();
            manager.RemoveScreen(this);

            for (const auto& screen : screensToLoad_)
            {
                if (screen)
                {
                    manager.AddScreen(screen, getControllingPlayerProperty());
                }
            }

            // Signal the background thread to exit, then wait for it to do so.
            if (backgroundThread)
            {
                backgroundThreadExit->Set();
                backgroundThread->join();
                backgroundThread.reset();
            }

            // Once the load has finished, we use ResetElapsedTime to tell
            // the  game timing mechanism that we have just finished a very
            // long frame, and that it should not try to catch up.
            manager.getGameProperty().ResetElapsedTime();
        }
    }

    void LoadingScreen::Draw(const GameTime& gameTime)
    {
        auto& manager = getScreenManagerProperty();

        // If we are the only active screen, that means all the previous screens
        // must have finished transitioning off. We check for this in the Draw
        // method, rather than in Update, because it isn't enough just for the
        // screens to be gone: in order for the transition to look good we must
        // have actually drawn a frame without them before we perform the load.
        if (getScreenStateProperty() == ScreenState::Active && manager.GetScreens().size() == 1)
            otherScreensAreGone_ = true;

        DrawLoadingMessage(gameTime);
    }

    // The loading text, drawn by Draw and by the worker. The worker must not read the screen list
    // (C# reads a copied array; here the main thread is changing it during the load).
    void LoadingScreen::DrawLoadingMessage(const GameTime& gameTime)
    {
        auto& manager = getScreenManagerProperty();

        // The gameplay screen takes a while to load, so we display a loading
        // message while that is going on, but the menus load very quickly, and
        // it would look silly if we flashed this up for just a fraction of a
        // second while returning from the game to the menus. This parameter
        // tells us how long the loading is going to take, so we know whether
        // to bother drawing the message.
        if (loadingIsSlow_)
        {
            auto& spriteBatch = manager.getSpriteBatchProperty();
            auto& font = manager.getFontProperty();

            std::string message = Resources::Loading;

            // Center the text in the viewport.
            const auto viewport = manager.getGraphicsDeviceProperty().getViewportProperty();
            const Vector2 viewportSize(static_cast<float>(viewport.getWidthProperty()), static_cast<float>(viewport.getHeightProperty()));
            const Vector2 textSize = font.MeasureString(message);
            const Vector2 textPosition = (viewportSize - textSize) / 2.0f;

            const Color color = Color::White * getTransitionAlphaProperty();

            // Animate the number of dots after our "Loading..." message.
            loadAnimationTimer = loadAnimationTimer + gameTime.getElapsedGameTimeProperty();

            const int dotCount = static_cast<int>(loadAnimationTimer.getTotalSecondsProperty() * 5) % 10;

            message += std::string(static_cast<std::size_t>(dotCount), '.');

            // Draw the text.
            spriteBatch.Begin();
            spriteBatch.DrawString(font, message, textPosition, color);
            spriteBatch.End();
        }
    }

    // Worker thread draws the loading animation and updates the network session while the load
    // is taking place.
    void LoadingScreen::BackgroundWorkerThread()
    {
        long long lastTime = System::Diagnostics::Stopwatch::GetTimestamp();

        // EventWaitHandle.WaitOne will return true if the exit signal has
        // been triggered, or false if the timeout has expired. We use the
        // timeout to update at regular intervals, then break out of the
        // loop when we are signalled to exit.
        while (!backgroundThreadExit->WaitOne(1000 / 30))
        {
            const GameTime gameTime = GetGameTime(lastTime);

            DrawLoadAnimation(gameTime);

            UpdateNetworkSession();
        }
    }

    // Works out how long it has been since the last background thread update.
    GameTime LoadingScreen::GetGameTime(long long& lastTime)
    {
        const long long currentTime = System::Diagnostics::Stopwatch::GetTimestamp();
        const long long elapsedTicks = currentTime - lastTime;
        lastTime = currentTime;

        const System::TimeSpan elapsedTime = System::TimeSpan::FromTicks(
            elapsedTicks * System::TimeSpan::TicksPerSecond / System::Diagnostics::Stopwatch::Frequency);

        return GameTime(loadStartTime.getTotalGameTimeProperty() + elapsedTime, elapsedTime);
    }

    // Calls directly into our Draw method from the background worker thread, so as to update the
    // load animation in parallel with the actual loading.
    void LoadingScreen::DrawLoadAnimation(const GameTime& gameTime)
    {
        if ((graphicsDevice == nullptr) || graphicsDevice->getIsDisposedProperty())
            return;

        try
        {
            graphicsDevice->Clear(Color::Black);

            // Draw the loading screen.
            DrawLoadingMessage(gameTime);

            // If we have a message display component, we want to display
            // that over the top of the loading screen, too.
            if (messageDisplay != nullptr)
            {
                GameTime updateTime = gameTime;
                messageDisplay->Update(updateTime);
                messageDisplay->Draw(gameTime);
            }

            graphicsDevice->Present();
        }
        catch (...)
        {
            // If anything went wrong (for instance the graphics device was lost
            // or reset) we don't have any good way to recover while running on a
            // background thread. Setting the device to null will stop us from
            // rendering, so the main game can deal with the problem later on.
            graphicsDevice = nullptr;
        }
    }

    // Updates the network session from the background worker thread, to avoid disconnecting due to
    // network timeouts even if loading takes a long time.
    void LoadingScreen::UpdateNetworkSession()
    {
        if ((networkSession == nullptr) || (networkSession->getSessionStateProperty() == NetworkSessionState::Ended))
            return;

        try
        {
            networkSession->Update();
        }
        catch (...)
        {
            // If anything went wrong, we don't have a good way to report that
            // error while running on a background thread. Setting the session to
            // null will stop us from updating it, so the main game can deal with
            // the problem later on.
            networkSession = nullptr;
        }
    }

    const std::string& LoadingScreen::GetTypeName() const
    {
        static const std::string name = "NetworkStateManagement.LoadingScreen";
        return name;
    }
}
