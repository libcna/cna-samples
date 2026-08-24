// SPDX-License-Identifier: MS-PL

#include "FullScreenSplash.hpp"

#include "SpacewarGame.hpp"
#include "XInputHelper.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
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

    FullScreenSplash::FullScreenSplash(Game* game, std::string textureName)
        : Screen(game)
    {
        SetTexture(std::move(textureName));
    }

    FullScreenSplash::FullScreenSplash(Game* game, std::string textureName,
                                       System::TimeSpan timeoutSpan, GameState nextState)
        : Screen(game), timeout_(timeoutSpan.getTotalSecondsProperty()), nextState_(nextState)
    {
        SetTexture(std::move(textureName));
    }

    void FullScreenSplash::SetTexture(std::string textureName)
    {
        textureName_ = std::move(textureName);
    }

    GameState FullScreenSplash::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        if (endTime_ < 0.0)
            endTime_ = time.getTotalSecondsProperty() + timeout_;

        if (nextState_ != GameState::None &&
            ((timeout_ != 0.0 && time.getTotalSecondsProperty() > endTime_) ||
             XInputHelper::getGamePadsProperty()[PlayerIndex::One].getAPressedProperty() ||
             XInputHelper::getGamePadsProperty()[PlayerIndex::Two].getAPressedProperty()))
            return nextState_;
        return Screen::Update(time, elapsedTime);
    }

    void FullScreenSplash::Render()
    {
        auto& device = game_->getGraphicsDeviceProperty();
        batch_->Begin(SpriteSortMode::Deferred, BlendState::Opaque);
        device.setDepthStencilStateProperty(DepthStencilState::DepthRead);
        auto texture = SpacewarGame::getContentManagerProperty().Load<Texture2D>(
            SpacewarGame::getSettingsProperty().MediaPath + textureName_);
        batch_->Draw(texture, Vector2::Zero, std::nullopt, Color::White);
        batch_->End();
        Screen::Render();
    }
}
