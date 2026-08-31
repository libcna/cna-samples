// SPDX-License-Identifier: MS-PL
#pragma once

// BlackjackGame.hpp -- C++ port of BlackjackGame.cs (XNA 4.0 CardsStarterKit
// sample). The top-level Game class: sets up the GraphicsDeviceManager,
// ScreenManager, and initial screen stack, and initializes AudioManager.

#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#if defined(XBOX)
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#endif

namespace GameStateManagement {
class ScreenManager;
}

namespace Blackjack {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GraphicsDeviceManager;
class BlackjackGame : public Game {
public:
    inline static float HeightScale = 1.0f;
    inline static float WidthScale = 1.0f;

    BlackjackGame();
    ~BlackjackGame() override;

    CNAEXT const std::string& GetTypeName() const override;

protected:
    void Initialize() override;
    void LoadContent() override;

private:
    GraphicsDeviceManager graphics_;
    std::shared_ptr<GameStateManagement::ScreenManager> screenManager_;
#if defined(XBOX)
    std::unique_ptr<Microsoft::Xna::Framework::GamerServices::GamerServicesComponent> gamerServices_;
#endif
};

} // namespace Blackjack
