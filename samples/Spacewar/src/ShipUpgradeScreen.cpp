// SPDX-License-Identifier: MS-PL

#include "ShipUpgradeScreen.hpp"

#include <algorithm>
#include <array>
#include <cmath>

#include "EvolvedShape.hpp"
#include "Font.hpp"
#include "Sound.hpp"
#include "SpacewarGame.hpp"
#include "XInputHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
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

    namespace
    {
        const std::array<std::string, 4> UpgradeScoreLookup{"000", "001", "011", "111"};
    }

    ShipUpgradeScreen::ShipUpgradeScreen(Game* game) : Screen(game)
    {
        menuMusic_ = Sound::Play(Sounds::MenuMusic);
        auto weapon1 = std::make_unique<SceneItem>(game,
            std::make_unique<EvolvedShape>(game, EvolvedShapes::Weapon, PlayerIndex::One,
                                           static_cast<int>(ProjectileType::Peashooter), LightingType::Menu),
            Vector3(-170, -30, 0));
        weapons_[0] = weapon1.get();
        weapons_[0]->setScaleProperty(Vector3(0.06f));
        scene_->Add(std::move(weapon1));
        auto weapon2 = std::make_unique<SceneItem>(game,
            std::make_unique<EvolvedShape>(game, EvolvedShapes::Weapon, PlayerIndex::Two,
                                           static_cast<int>(ProjectileType::Peashooter), LightingType::Menu),
            Vector3(170, -30, 0));
        weapons_[1] = weapon2.get();
        weapons_[1]->setScaleProperty(Vector3(0.06f));
        scene_->Add(std::move(weapon2));
    }

    GameState ShipUpgradeScreen::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
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
                auto& pad = pads[static_cast<PlayerIndex>(player)];
                for (int weapon = 1; weapon < 5; ++weapon)
                {
                    bool pressed = false;
                    if (weapon == 1) pressed = pad.getUpPressedProperty();
                    if (weapon == 2) pressed = pad.getLeftPressedProperty();
                    if (weapon == 3) pressed = pad.getRightPressedProperty();
                    if (weapon == 4) pressed = pad.getDownPressedProperty();
                    auto& playerInfo = SpacewarGame::getPlayersProperty()[player];
                    const int cost = SpacewarGame::getSettingsProperty().Weapons[weapon].Cost;
                    if (pressed && playerInfo.getCashProperty() >= cost)
                    {
                        Sound::PlayCue(Sounds::MenuAdvance);
                        purchasedWeapon_[player] = static_cast<ProjectileType>(weapon);
                        playerInfo.setCashProperty(playerInfo.getCashProperty() - cost);
                        playerInfo.setProjectileTypeProperty(static_cast<ProjectileType>(weapon));
                        weapons_[player]->setShapeItemProperty(std::make_unique<EvolvedShape>(
                            game_, EvolvedShapes::Weapon, static_cast<PlayerIndex>(player), weapon, LightingType::Menu));
                        flashEndTime_[player] = time + flashTime_;
                    }
                }
            }
        }

        bool anyPlayersCounting = false;
        for (int player = 0; player < 2; ++player)
        {
            const int cash = SpacewarGame::getPlayersProperty()[player].getCashProperty();
            if (playerCashCount_[player] != cash)
            {
                anyPlayersCounting = true;
                if (!playingTallySound_)
                {
                    tallySound_ = Sound::Play(Sounds::PointsTally);
                    playingTallySound_ = true;
                }
                const int difference = cash - playerCashCount_[player];
                const int direction = (difference > 0) - (difference < 0);
                playerCashCount_[player] += static_cast<int>(
                    elapsedTime.getTotalMillisecondsProperty() * CountSpeed * direction);
                if (direction == 1) playerCashCount_[player] = std::min(playerCashCount_[player], cash);
                else playerCashCount_[player] = std::max(playerCashCount_[player], cash);
            }
        }
        if (!anyPlayersCounting && playingTallySound_)
        {
            Sound::Stop(tallySound_.get());
            tallySound_.reset();
            playingTallySound_ = false;
        }

        for (int i = 0; i < 2; ++i)
            weapons_[i]->setRotationProperty(Vector3(-0.3f,
                static_cast<float>(time.getTotalSecondsProperty()) * static_cast<float>(i * 2 - 1), 0));
        for (int player = 0; player < 2; ++player)
        {
            if (flashEndTime_[player] > time)
                flashPercent_[player] = static_cast<float>(
                    std::sin((time - flashEndTime_[player]).getTotalSecondsProperty() * 40.0) * 0.5 + 1.0);
            else flashPercent_[player] = 1.0f;
        }
        Screen::Update(time, elapsedTime);

        if (player1Ready_ && player2Ready_)
        {
            for (int i = 0; i < 2; ++i)
                SpacewarGame::getPlayersProperty()[i].setProjectileTypeProperty(purchasedWeapon_[i]);
            Shutdown();
            return GameState::PlayEvolved;
        }
        return GameState::None;
    }

    void ShipUpgradeScreen::Shutdown()
    {
        if (playingTallySound_) Sound::Stop(tallySound_.get());
        Sound::Stop(menuMusic_.get());
        Screen::Shutdown();
    }

    std::string ShipUpgradeScreen::Grouped(int value)
    {
        std::string digits = System::Int32::ToString(value);
        for (std::ptrdiff_t i = static_cast<std::ptrdiff_t>(digits.size()) - 3; i > 0; i -= 3)
            digits.insert(static_cast<std::size_t>(i), ",");
        return digits;
    }

    void ShipUpgradeScreen::Render()
    {
        auto& device = game_->getGraphicsDeviceProperty();
        auto mainTexture = SpacewarGame::getContentManagerProperty().Load<Texture2D>(
            SpacewarGame::getSettingsProperty().MediaPath + UpgradeTexture);
        batch_->Begin(SpriteSortMode::Texture, BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        batch_->Draw(mainTexture, Vector2::Zero, Rectangle(0, 0, 1280, 720), Color::White);
        batch_->End();
        batch_->Begin(SpriteSortMode::Texture, BlendState::AlphaBlend);
        for (int player = 0; player < 2; ++player)
        {
            for (int weapon = 1; weapon < 5; ++weapon)
            {
                const Vector2 position = weaponPositions_[player][weapon - 1];
                if ((player1Ready_ && player == 0) || (player2Ready_ && player == 1))
                    batch_->Draw(mainTexture, position, ReadySprite(player, weapon), Color::White);
                else if (static_cast<int>(purchasedWeapon_[player]) == weapon)
                    batch_->Draw(mainTexture, position, SelectedSprite(player, weapon),
                                 Color(Vector4(1, 1, 1, flashPercent_[player])));
                else if (SpacewarGame::getPlayersProperty()[player].getCashProperty() <
                         SpacewarGame::getSettingsProperty().Weapons[weapon].Cost)
                    batch_->Draw(mainTexture, position, DisabledSprite(player, weapon), Color::White);
            }
        }
        if (player1Ready_)
            batch_->Draw(mainTexture, Vector2(55, 620), Rectangle(10, 1205, 190, 70), Color::White);
        if (player2Ready_)
            batch_->Draw(mainTexture, Vector2(1040, 620), Rectangle(330, 1205, 190, 70), Color::White);
        batch_->End();

        Font::Begin();
        for (int offset = 0; offset < 621; offset += 620)
        {
            if ((!player1Ready_ && offset == 0) || (!player2Ready_ && offset == 620))
            {
                Font::Draw(FontStyle::WeaponSmall, 296 + offset, 255,
                    "$" + Grouped(SpacewarGame::getSettingsProperty().Weapons[1].Cost), upgradeFontColor_);
                Font::Draw(FontStyle::WeaponSmall, 110 + offset, 438,
                    "$" + Grouped(SpacewarGame::getSettingsProperty().Weapons[2].Cost), upgradeFontColor_);
                Font::Draw(FontStyle::WeaponSmall, 480 + offset, 438,
                    "$" + Grouped(SpacewarGame::getSettingsProperty().Weapons[3].Cost), upgradeFontColor_);
                Font::Draw(FontStyle::WeaponSmall, 296 + offset, 621,
                    "$" + Grouped(SpacewarGame::getSettingsProperty().Weapons[4].Cost), upgradeFontColor_);
            }
        }
        Font::Draw(FontStyle::WeaponLarge, 322, 40, "$", upgradeFontColor_);
        Font::Draw(FontStyle::WeaponLarge, 346, 40, Grouped(playerCashCount_[0]), upgradeFontColor_);
        Font::Draw(FontStyle::WeaponLarge, 840, 40, "$", upgradeFontColor_);
        Font::Draw(FontStyle::WeaponLarge, 866, 40, Grouped(playerCashCount_[1]), upgradeFontColor_);
        Font::Draw(FontStyle::ScoreButtons, 409, 94,
            UpgradeScoreLookup[SpacewarGame::getPlayersProperty()[0].getScoreProperty()]);
        Font::Draw(FontStyle::ScoreButtons, 789, 94,
            UpgradeScoreLookup[SpacewarGame::getPlayersProperty()[1].getScoreProperty()]);
        Font::End();
        Screen::Render();
    }

    Rectangle ShipUpgradeScreen::SelectedSprite(int player, int weapon)
    {
        return Rectangle(10 + 320 * (weapon - 1), 730 + player * 160, 150, 150);
    }
    Rectangle ShipUpgradeScreen::DisabledSprite(int player, int weapon)
    {
        return Rectangle(170 + 320 * (weapon - 1), 730 + player * 160, 150, 150);
    }
    Rectangle ShipUpgradeScreen::ReadySprite(int player, int weapon)
    {
        return Rectangle(10 + 160 * (weapon - 1) + 640 * player, 1050, 150, 150);
    }

    void ShipUpgradeScreen::OnCreateDevice()
    {
        Screen::OnCreateDevice();
        weapons_[0]->getShapeItemProperty()->OnCreateDevice();
        weapons_[1]->getShapeItemProperty()->OnCreateDevice();
    }
}
