// SPDX-License-Identifier: MS-PL

#include "TitleScreen.hpp"

#include "SpacewarGame.hpp"
#include "XInputHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteSortMode.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    TitleScreen::TitleScreen(Game* game)
        : FullScreenSplash(game, "textures/spacewar_title_FINAL", System::TimeSpan::Zero,
                           GameState::ShipSelection)
    {
    }

    GameState TitleScreen::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        if (XInputHelper::getGamePadsProperty()[PlayerIndex::One].getXPressedProperty() ||
            XInputHelper::getGamePadsProperty()[PlayerIndex::Two].getXPressedProperty())
            showInfo_ = true;

        if (XInputHelper::getGamePadsProperty()[PlayerIndex::One].getBPressedProperty() ||
            XInputHelper::getGamePadsProperty()[PlayerIndex::Two].getBPressedProperty())
        {
            if (showInfo_)
                showInfo_ = false;
            else
                playRetro_ = true;
        }

        if (showInfo_)
            return GameState::None;

        GameState result = FullScreenSplash::Update(time, elapsedTime);
        if (playRetro_)
            result = GameState::PlayRetro;
        return result;
    }

    void TitleScreen::Render()
    {
        FullScreenSplash::Render();
        auto& device = game_->getGraphicsDeviceProperty();
        batch_->Begin(SpriteSortMode::Deferred, BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::None);

        if (showInfo_)
        {
            auto texture = SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                SpacewarGame::getSettingsProperty().MediaPath + "textures/info_screen");
            batch_->Draw(texture, Vector2(270.0f, 135.0f), std::nullopt, Color::White);
        }
        else
        {
            auto texture = SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                SpacewarGame::getSettingsProperty().MediaPath + "textures/title_button_overlay");
            batch_->Draw(texture, Vector2(950.0f, 450.0f), std::nullopt, Color::White);
        }
        batch_->End();
    }

    void TitleScreen::OnCreateDevice()
    {
        FullScreenSplash::OnCreateDevice();
    }
}
