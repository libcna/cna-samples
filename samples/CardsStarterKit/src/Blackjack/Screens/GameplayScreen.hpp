// SPDX-License-Identifier: MS-PL
#pragma once

// GameplayScreen.hpp -- C++ port of Screens/GameplayScreen.cs (XNA 4.0
// CardsStarterKit sample). Hosts a BlackjackCardGame instance and forwards
// input/update/draw to it; also implements pause (hide/disable gameplay
// components, show PauseScreen) and resume.

#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/DrawableGameComponent.hpp"
#if defined(XBOX)
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#endif

#include "../../GameStateManagement/GameScreen.hpp"
#include "../../GameStateManagement/ScreenManager.hpp"
#include "../../BlackjackGame.hpp"
#include "../BetGameComponent.hpp"
#include "../BlackjackCardGame.hpp"
#include "../BlackjackCommon.hpp"
#include "../InputHelper.hpp"
#include "BackgroundScreen.hpp"

namespace Blackjack {

using GameStateManagement::GameScreen;
using GameStateManagement::InputState;
using GameStateManagement::ScreenManager;
using Microsoft::Xna::Framework::DrawableGameComponent;

class PauseScreen;  // forward declaration (Screens/PauseScreen.hpp)

class GameplayScreen : public GameScreen {
public:
    explicit GameplayScreen(const std::string& theme) : theme_(theme) {
        setTransitionOnTime(TimeSpan::FromSeconds(0.0));
        setTransitionOffTime(TimeSpan::FromSeconds(0.5));
#if defined(WINDOWS_PHONE)
        setEnabledGestures(Microsoft::Xna::Framework::Input::Touch::GestureType::Tap);
#endif
    }

    void LoadContent() override {
        safeArea_ = GetScreenManager()->SafeArea();

        inputHelper_ = std::make_shared<InputHelper>(GetScreenManager()->getGameProperty());
        inputHelper_->setDrawOrderProperty(1000);
        GetScreenManager()->getGameProperty().getComponentsProperty().Add(inputHelper_.get());
#if !defined(XBOX)
        inputHelper_->setVisibleProperty(false);
        inputHelper_->setEnabledProperty(false);
#endif

        blackJackGame_ = std::make_shared<BlackjackCardGame>(
            GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty().getBoundsProperty(),
            Vector2((float)(safeArea_.X + safeArea_.Width / 2 - 50), (float)(safeArea_.Y + 20)),
            [this](int player) { return GetPlayerCardPosition(player); }, *GetScreenManager(), theme_);

        InitializeGame();
    }

    void UnloadContent() override {
        (void)GetScreenManager()->getGameProperty().getComponentsProperty().Remove(inputHelper_.get());
    }

    void HandleInput(InputState& input) override {
        if (input.IsPauseGame(std::nullopt))
            PauseCurrentGame();
        GameScreen::HandleInput(input);
    }

    void Update(GameTime& gameTime, bool otherScreenHasFocus, bool coveredByOtherScreen) override {
#if defined(XBOX)
        if (Microsoft::Xna::Framework::GamerServices::Guide::getIsVisibleProperty())
            PauseCurrentGame();
#endif
        if (blackJackGame_ && !coveredByOtherScreen)
            blackJackGame_->Update(gameTime);

        GameScreen::Update(gameTime, otherScreenHasFocus, coveredByOtherScreen);
    }

    void Draw(const GameTime& gameTime) override {
        GameScreen::Draw(gameTime);
        if (blackJackGame_)
            blackJackGame_->Draw(gameTime);
    }

    // Reveals and re-enables components hidden by PauseCurrentGame().
    void ReturnFromPause() {
        for (auto* component : pauseEnabledComponents_)
            component->setEnabledProperty(true);
        for (auto* component : pauseVisibleComponents_)
            component->setVisibleProperty(true);
    }

private:
    void InitializeGame() {
        blackJackGame_->Initialize();

        blackJackGame_->AddPlayer(std::make_shared<BlackjackPlayer>("Abe", blackJackGame_.get()));

        auto benny = std::make_shared<BlackjackAIPlayer>("Benny", blackJackGame_.get());
        blackJackGame_->AddPlayer(benny);
        benny->Hit.Add([this](System::Object* sender, const System::EventArgs& e) { player_Hit(sender, e); });
        benny->Stand.Add([this](System::Object* sender, const System::EventArgs& e) { player_Stand(sender, e); });

        auto chuck = std::make_shared<BlackjackAIPlayer>("Chuck", blackJackGame_.get());
        blackJackGame_->AddPlayer(chuck);
        chuck->Hit.Add([this](System::Object* sender, const System::EventArgs& e) { player_Hit(sender, e); });
        chuck->Stand.Add([this](System::Object* sender, const System::EventArgs& e) { player_Stand(sender, e); });

        std::string assets[] = {"blackjack", "bust", "lose", "push", "win", "pass", "shuffle_" + theme_};
        for (auto& asset : assets)
            blackJackGame_->LoadUITexture("UI", asset);

        blackJackGame_->StartRound();
    }

    Vector2 GetPlayerCardPosition(int player) const {
        const Vector2 offsets[] = {
            Vector2(100.0f * BlackjackGame::WidthScale, 190.0f * BlackjackGame::HeightScale),
            Vector2(336.0f * BlackjackGame::WidthScale, 210.0f * BlackjackGame::HeightScale),
            Vector2(570.0f * BlackjackGame::WidthScale, 190.0f * BlackjackGame::HeightScale),
        };
        if (player < 0 || player > 2)
            throw std::invalid_argument("Player index should be between 0 and 2");

        Rectangle safeArea = GetScreenManager()->SafeArea();
        return Vector2(static_cast<float>(safeArea.X),
                       static_cast<float>(safeArea.Y) + 200.0f * (BlackjackGame::HeightScale - 1.0f))
            + offsets[player];
    }

    void player_Stand(System::Object* sender, const System::EventArgs& e) {
        (void)sender;
        (void)e;
        blackJackGame_->Stand();
    }

    void player_Split(System::Object* sender, const System::EventArgs& e) {
        (void)sender;
        (void)e;
        blackJackGame_->Split();
    }

    void player_Hit(System::Object* sender, const System::EventArgs& e) {
        (void)sender;
        (void)e;
        blackJackGame_->Hit();
    }

    void player_Double(System::Object* sender, const System::EventArgs& e) {
        (void)sender;
        (void)e;
        blackJackGame_->Double();
    }

    void PauseCurrentGame();  // defined in PauseScreen.hpp (needs PauseScreen)

    std::shared_ptr<BlackjackCardGame> blackJackGame_;
    std::shared_ptr<InputHelper> inputHelper_;
    std::string theme_;
    std::vector<DrawableGameComponent*> pauseEnabledComponents_;
    std::vector<DrawableGameComponent*> pauseVisibleComponents_;
    Rectangle safeArea_;
};

} // namespace Blackjack
