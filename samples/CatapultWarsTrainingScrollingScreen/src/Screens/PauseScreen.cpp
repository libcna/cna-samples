// SPDX-License-Identifier: MS-PL

#include "PauseScreen.hpp"

#include "MainMenuScreen.hpp"

namespace CatapultGame {

void PauseScreen::OnCancel(PlayerIndex playerIndex) {
    (void)playerIndex;
    AudioManager::StopSounds();
    GetScreenManager()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    ExitScreen();
}

} // namespace CatapultGame
