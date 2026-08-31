// SPDX-License-Identifier: MS-PL

#include "GameplayScreen.hpp"

#include "BackgroundScreen.hpp"
#include "PauseScreen.hpp"

namespace CatapultGame {

void GameplayScreen::PauseCurrentGame() {
    auto pauseMenuBackground = std::make_shared<BackgroundScreen>();

    if (isDragging_) {
        isDragging_ = false;
        player_->getCatapultProperty()->setCurrentStateProperty(CatapultState::Idle);
    }

    GetScreenManager()->AddScreen(pauseMenuBackground, std::nullopt);
    GetScreenManager()->AddScreen(
        std::make_shared<PauseScreen>(pauseMenuBackground, player_.get(), computer_.get()),
        std::nullopt);
}

} // namespace CatapultGame
