// SPDX-License-Identifier: MS-PL

#include "SelectionScreen.hpp"

#include "EvolvedShape.hpp"
#include "Font.hpp"
#include "Sound.hpp"
#include "SpacewarGame.hpp"
#include "XInputHelper.hpp"
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
#include "System/Int32.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::Graphics;

    SelectionScreen::SelectionScreen(Game* game) : Screen(game)
    {
        menuMusic_ = Sound::Play(Sounds::MenuMusic);
        auto ship1 = std::make_unique<SceneItem>(game,
            std::make_unique<EvolvedShape>(game, EvolvedShapes::Ship, PlayerIndex::One, 0, 0, LightingType::Menu),
            Vector3(-120, 0, 0));
        ships_[0] = ship1.get();
        ships_[0]->setScaleProperty(Vector3(0.05f));
        scene_->Add(std::move(ship1));
        auto ship2 = std::make_unique<SceneItem>(game,
            std::make_unique<EvolvedShape>(game, EvolvedShapes::Ship, PlayerIndex::Two, 0, 0, LightingType::Menu),
            Vector3(120, 0, 0));
        ships_[1] = ship2.get();
        ships_[1]->setScaleProperty(Vector3(0.05f));
        scene_->Add(std::move(ship2));
    }

    GameState SelectionScreen::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        auto& pads = XInputHelper::getGamePadsProperty();
        if (pads[PlayerIndex::One].getAPressedProperty() ||
            (!pads[PlayerIndex::One].getStateProperty().getIsConnectedProperty() &&
             SpacewarGame::getCurrentPlatformProperty() != System::PlatformID::Win32NT))
            player1Ready_ = true;
        if (pads[PlayerIndex::Two].getAPressedProperty() ||
            (!pads[PlayerIndex::Two].getStateProperty().getIsConnectedProperty() &&
             SpacewarGame::getCurrentPlatformProperty() != System::PlatformID::Win32NT))
            player2Ready_ = true;
        if (pads[PlayerIndex::One].getBPressedProperty()) player1Ready_ = false;
        if (pads[PlayerIndex::Two].getBPressedProperty()) player2Ready_ = false;

        for (int player = 0; player < 2; ++player)
        {
            if ((!player1Ready_ && player == 0) || (!player2Ready_ && player == 1))
            {
                const int ship = selectedShip_[player];
                const int skin = selectedSkin_[player];
                auto& pad = pads[static_cast<PlayerIndex>(player)];
                if (pad.getUpPressedProperty()) selectedShip_[player] += 5;
                if (pad.getDownPressedProperty()) ++selectedShip_[player];
                if (pad.getLeftPressedProperty()) selectedSkin_[player] += 5;
                if (pad.getRightPressedProperty()) ++selectedSkin_[player];
                selectedShip_[player] %= 3;
                selectedSkin_[player] %= 3;
                if (ship != selectedShip_[player] || skin != selectedSkin_[player])
                {
                    Sound::PlayCue(Sounds::MenuScroll);
                    ships_[player]->setShapeItemProperty(std::make_unique<EvolvedShape>(
                        game_, EvolvedShapes::Ship, static_cast<PlayerIndex>(player),
                        selectedShip_[player], selectedSkin_[player], LightingType::Menu));
                }
            }
        }
        for (int i = 0; i < 2; ++i)
            ships_[i]->setRotationProperty(Vector3(-0.3f,
                static_cast<float>(time.getTotalSecondsProperty()) * static_cast<float>(i * 2 - 1), 0));
        Screen::Update(time, elapsedTime);

        if (player1Ready_ && player2Ready_)
        {
            for (int i = 0; i < 2; ++i)
            {
                SpacewarGame::getPlayersProperty()[i].setShipClassProperty(static_cast<ShipClass>(selectedShip_[i]));
                SpacewarGame::getPlayersProperty()[i].setSkinProperty(selectedSkin_[i]);
            }
            Shutdown();
            return GameState::PlayEvolved;
        }
        return GameState::None;
    }

    void SelectionScreen::Shutdown()
    {
        Sound::Stop(menuMusic_.get());
        Screen::Shutdown();
    }

    void SelectionScreen::Render()
    {
        auto& device = game_->getGraphicsDeviceProperty();
        auto mainTexture = SpacewarGame::getContentManagerProperty().Load<Texture2D>(
            SpacewarGame::getSettingsProperty().MediaPath + SelectionTexture);
        batch_->Begin(SpriteSortMode::Deferred, BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        batch_->Draw(mainTexture, Vector2::Zero, Rectangle(0, 0, 1280, 720), Color::White);
        batch_->End();
        batch_->Begin(SpriteSortMode::Texture, BlendState::AlphaBlend);

        if (player1Ready_)
        {
            batch_->Draw(mainTexture, Vector2(50, 610), Rectangle(960, 1095, 190, 80), Color::White);
            batch_->Draw(mainTexture, Vector2(10, 127), Rectangle(594, 911, 290, 112), Color::White);
            batch_->Draw(mainTexture, Vector2(10, 239), Rectangle(594, 1050, 290, 180), Color::White);
            batch_->Draw(mainTexture, Vector2(10, 419), Rectangle(10, 1200, 290, 140), Color::White);
            batch_->Draw(mainTexture, Vector2(331, 573), Rectangle(960, 911, 68, 70), Color::White);
            batch_->Draw(mainTexture, Vector2(399, 573), Rectangle(1040, 911, 68, 70), Color::White);
            batch_->Draw(mainTexture, Vector2(467, 573), Rectangle(1120, 911, 68, 70), Color::White);
        }
        else
        {
            const std::array<Rectangle, 3> shipSources{
                Rectangle(10, 730, 290, 112), Rectangle(10, 860, 290, 180), Rectangle(10, 1050, 290, 140)};
            const std::array<Vector2, 3> shipPositions{Vector2(10, 127), Vector2(10, 239), Vector2(10, 419)};
            batch_->Draw(mainTexture, shipPositions[selectedShip_[0]], shipSources[selectedShip_[0]], Color::White);
            const std::array<Vector2, 3> skinPositions{Vector2(331, 573), Vector2(399, 573), Vector2(467, 573)};
            const std::array<Rectangle, 3> skinSources{
                Rectangle(960, 730, 68, 70), Rectangle(1040, 730, 68, 70), Rectangle(1120, 730, 68, 70)};
            batch_->Draw(mainTexture, skinPositions[selectedSkin_[0]], skinSources[selectedSkin_[0]], Color::White);
        }

        if (player2Ready_)
        {
            batch_->Draw(mainTexture, Vector2(1040, 610), Rectangle(960, 1200, 190, 80), Color::White);
            batch_->Draw(mainTexture, Vector2(960, 127), Rectangle(594, 1290, 290, 112), Color::White);
            batch_->Draw(mainTexture, Vector2(1040, 239), Rectangle(332, 1050, 225, 180), Color::White);
            batch_->Draw(mainTexture, Vector2(1040, 419), Rectangle(331, 1290, 225, 140), Color::White);
            batch_->Draw(mainTexture, Vector2(745, 573), Rectangle(960, 1000, 68, 70), Color::White);
            batch_->Draw(mainTexture, Vector2(813, 573), Rectangle(1040, 1000, 68, 70), Color::White);
            batch_->Draw(mainTexture, Vector2(881, 573), Rectangle(1120, 1000, 68, 70), Color::White);
        }
        else
        {
            const std::array<Vector2, 3> shipPositions{Vector2(960, 127), Vector2(1040, 239), Vector2(1040, 419)};
            const std::array<Rectangle, 3> shipSources{
                Rectangle(331, 730, 290, 112), Rectangle(331, 860, 225, 180), Rectangle(700, 730, 225, 140)};
            batch_->Draw(mainTexture, shipPositions[selectedShip_[1]], shipSources[selectedShip_[1]], Color::White);
            const std::array<Vector2, 3> skinPositions{Vector2(745, 573), Vector2(813, 573), Vector2(881, 573)};
            const std::array<Rectangle, 3> skinSources{
                Rectangle(960, 820, 68, 70), Rectangle(1040, 820, 68, 70), Rectangle(1120, 820, 68, 70)};
            batch_->Draw(mainTexture, skinPositions[selectedSkin_[1]], skinSources[selectedSkin_[1]], Color::White);
        }
        batch_->End();
        Font::Begin();
        Font::Draw(FontStyle::ShipNames, 331, 500, System::Int32::ToString(selectedShip_[0]), Vector4(0.2f, 0.89f, 1, 1));
        Font::Draw(FontStyle::ShipNames, 745, 500, System::Int32::ToString(selectedShip_[1]), Vector4(1, 0.733f, 0.392f, 1));
        Font::End();
        Screen::Render();
    }

    void SelectionScreen::OnCreateDevice()
    {
        Screen::OnCreateDevice();
        ships_[0]->getShapeItemProperty()->OnCreateDevice();
        ships_[1]->getShapeItemProperty()->OnCreateDevice();
    }
}
