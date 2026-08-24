// SPDX-License-Identifier: MS-PL

#include "Screen.hpp"

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"

namespace Spacewar
{
    Screen::Screen(Microsoft::Xna::Framework::Game* game)
        : scene_(std::make_unique<SceneItem>(game)), game_(game)
    {
        if (game_)
            batch_ = std::make_unique<Microsoft::Xna::Framework::Graphics::SpriteBatch>(game_->getGraphicsDeviceProperty());
    }

    Screen::~Screen() = default;

    Microsoft::Xna::Framework::Game* Screen::getGameInstanceProperty() const { return game_; }
    Microsoft::Xna::Framework::Graphics::SpriteBatch* Screen::getSpriteBatchProperty() const { return batch_.get(); }

    GameState Screen::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        scene_->Update(time, elapsedTime);
        return overlay_ ? overlay_->Update(time, elapsedTime) : GameState::None;
    }

    void Screen::Render()
    {
        scene_->Render();
        if (overlay_)
            overlay_->Render();
    }

    void Screen::Shutdown()
    {
        if (overlay_)
            overlay_->Shutdown();
        if (batch_)
            batch_->Dispose();
        batch_.reset();
    }

    void Screen::OnCreateDevice()
    {
        batch_ = std::make_unique<Microsoft::Xna::Framework::Graphics::SpriteBatch>(game_->getGraphicsDeviceProperty());
    }
}
