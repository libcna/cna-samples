// SPDX-License-Identifier: MS-PL

#include "VictoryScreen.hpp"

#include "EvolvedShape.hpp"
#include "Sound.hpp"
#include "SpacewarGame.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
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

    VictoryScreen::VictoryScreen(Game* game)
        : FullScreenSplash(game, VictoryTexture, System::TimeSpan::Zero, GameState::LogoSplash)
    {
        Sound::PlayCue(Sounds::TitleMusic);
        winningPlayerNumber_ = SpacewarGame::getPlayersProperty()[0].getScoreProperty() >
                               SpacewarGame::getPlayersProperty()[1].getScoreProperty() ? 0 : 1;
        const Player& winner = SpacewarGame::getPlayersProperty()[winningPlayerNumber_];
        auto ship = std::make_unique<SceneItem>(game,
            std::make_unique<EvolvedShape>(game, EvolvedShapes::Ship,
                winningPlayerNumber_ == 0 ? PlayerIndex::One : PlayerIndex::Two,
                static_cast<int>(winner.getShipClassProperty()), winner.getSkinProperty(), LightingType::Menu),
            Vector3(-90, -30, 0));
        ship_ = ship.get();
        ship_->setScaleProperty(Vector3(0.07f));
        scene_->Add(std::move(ship));
    }

    GameState VictoryScreen::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        ship_->setRotationProperty(Vector3(-0.3f, static_cast<float>(time.getTotalSecondsProperty()), 0));
        return FullScreenSplash::Update(time, elapsedTime);
    }

    void VictoryScreen::Render()
    {
        FullScreenSplash::Render();
        if (winningPlayerNumber_ == 1)
        {
            auto mainTexture = SpacewarGame::getContentManagerProperty().Load<Texture2D>(
                SpacewarGame::getSettingsProperty().MediaPath + VictoryTexture);
            batch_->Begin(SpriteSortMode::Deferred, BlendState::Opaque);
            game_->getGraphicsDeviceProperty().setDepthStencilStateProperty(DepthStencilState::DepthRead);
            batch_->Draw(mainTexture, Vector2(320, 525), Rectangle(50, 730, 320, 85), Color::White);
            batch_->End();
        }
    }

    void VictoryScreen::OnCreateDevice()
    {
        FullScreenSplash::OnCreateDevice();
        ship_->getShapeItemProperty()->OnCreateDevice();
    }
}
