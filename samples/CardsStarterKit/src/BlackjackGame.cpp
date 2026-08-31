// SPDX-License-Identifier: MS-PL

#include "BlackjackGame.hpp"

#include <optional>

#include "Blackjack/AudioManager.hpp"
#include "Blackjack/Screens/BackgroundScreen.hpp"
#include "Blackjack/Screens/MainMenuScreen.hpp"
#include "Blackjack/Screens/OptionsMenu.hpp"
#include "Blackjack/Screens/PauseScreen.hpp"
#include "GameStateManagement/ScreenManager.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "System/TimeSpan.hpp"

namespace Blackjack {

using GameStateManagement::ScreenManager;
using Microsoft::Xna::Framework::Rectangle;

BlackjackGame::BlackjackGame() : graphics_(this) {
    getContentProperty().setRootDirectoryProperty("Content");

    CNAEXT ScreenManager::RegisterScreenType<BackgroundScreen>();
    CNAEXT ScreenManager::RegisterScreenType<MainMenuScreen>();
    CNAEXT ScreenManager::RegisterScreenType<OptionsMenu>();
    CNAEXT ScreenManager::RegisterScreenType<PauseScreen>();

    screenManager_ = std::make_shared<ScreenManager>(*this);
    screenManager_->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
    screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    getComponentsProperty().Add(screenManager_.get());

#if defined(WINDOWS_PHONE)
    setTargetElapsedTimeProperty(System::TimeSpan::FromTicks(333333));
    graphics_.setIsFullScreenProperty(true);
#elif defined(XBOX)
    gamerServices_ = std::make_unique<
        Microsoft::Xna::Framework::GamerServices::GamerServicesComponent>(*this);
    getComponentsProperty().Add(gamerServices_.get());
#else
    setIsMouseVisibleProperty(true);
#endif

    AudioManager::Initialize(*this);
}

BlackjackGame::~BlackjackGame() {
    AudioManager::Shutdown(*this);
}

const std::string& BlackjackGame::GetTypeName() const {
    static const std::string name = "Blackjack.BlackjackGame";
    return name;
}

void BlackjackGame::Initialize() {
    Game::Initialize();

#if defined(XBOX)
    const auto displayMode = graphics_.getGraphicsDeviceProperty()->getDisplayModeProperty();
    graphics_.setPreferredBackBufferHeightProperty(displayMode.getHeightProperty());
    graphics_.setPreferredBackBufferWidthProperty(displayMode.getWidthProperty());
#elif !defined(WINDOWS_PHONE)
    graphics_.setPreferredBackBufferHeightProperty(480);
    graphics_.setPreferredBackBufferWidthProperty(800);
#endif
    graphics_.ApplyChanges();

    const Rectangle bounds = getGraphicsDeviceProperty().getViewportProperty().getTitleSafeAreaProperty();
    HeightScale = static_cast<float>(bounds.Height) / 480.0f;
    WidthScale = static_cast<float>(bounds.Width) / 800.0f;
}

void BlackjackGame::LoadContent() {
    AudioManager::LoadSounds();
    Game::LoadContent();
}

} // namespace Blackjack
