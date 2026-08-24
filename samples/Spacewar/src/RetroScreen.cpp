// SPDX-License-Identifier: MS-PL

#include "RetroScreen.hpp"

#include "Font.hpp"
#include "RetroProjectiles.hpp"
#include "RetroStarfield.hpp"
#include "RetroSun.hpp"
#include "Ship.hpp"
#include "SpacewarGame.hpp"
#include "Sun.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    RetroScreen::RetroScreen(Game* game) : SpacewarScreen(game)
    {
        backdrop_ = scene_->Add(std::make_unique<SceneItem>(game, std::make_unique<RetroStarfield>(game)));
        auto bullets = std::make_unique<RetroProjectiles>(game);
        bullets_ = bullets.get();
        auto ship1 = std::make_unique<Ship>(game, PlayerIndex::One, Vector3(-250, 0, 0), bullets_);
        ship1_ = ship1.get();
        ship1_->setRadiusProperty(10.0f);
        scene_->Add(std::move(ship1));
        auto ship2 = std::make_unique<Ship>(game, PlayerIndex::Two, Vector3(250, 0, 0), bullets_);
        ship2_ = ship2.get();
        ship2_->setRadiusProperty(10.0f);
        scene_->Add(std::move(ship2));
        sun_ = scene_->Add(std::make_unique<Sun>(game, std::make_unique<RetroSun>(game),
            Vector3(SpacewarGame::getSettingsProperty().SunPosition, 0.0f)));
        scene_->Add(std::move(bullets));
        paused_ = false;
    }

    void RetroScreen::Render()
    {
        auto background = SpacewarGame::getContentManagerProperty().Load<Texture2D>(
            SpacewarGame::getSettingsProperty().MediaPath + "textures/retro_backdrop");
        batch_->Begin();
        batch_->Draw(background, Vector2(0, 0), std::nullopt, Color::White);
        batch_->End();
        game_->getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::None);
        SpacewarScreen::Render();
        Font::Begin();
        Font::Draw(FontStyle::WeaponLarge, 300, 15, player1Score_);
        Font::Draw(FontStyle::WeaponLarge, 940, 15, player2Score_);
        Font::End();
    }

    GameState RetroScreen::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        HandleCollisions(time);
        return SpacewarScreen::Update(time, elapsedTime);
    }

    void RetroScreen::OnCreateDevice()
    {
        SpacewarScreen::OnCreateDevice();
        bullets_->OnCreateDevice();
    }
}
