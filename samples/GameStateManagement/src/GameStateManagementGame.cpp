// SPDX-License-Identifier: MS-PL

#include "GameStateManagementGame.hpp"

#include <memory>
#include <optional>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "ScreenManager/ScreenManager.hpp"
#include "Screens/BackgroundScreen.hpp"
#include "Screens/MainMenuScreen.hpp"

namespace GameStateManagement
{
    using namespace Microsoft::Xna::Framework;
    using Graphics::Texture2D;

    const std::array<std::string, 1> GameStateManagementGame::preloadAssets_ = {"gradient"};

    GameStateManagementGame::GameStateManagementGame()
    {
        getContentProperty().setRootDirectoryProperty("Content");

        graphics_ = std::make_unique<GraphicsDeviceManager>(this);
        graphics_->setPreferredBackBufferWidthProperty(853);
        graphics_->setPreferredBackBufferHeightProperty(480);

        screenManager_ = std::make_unique<ScreenManager>(*this);
        getComponentsProperty().Add(screenManager_.get());

        screenManager_->AddScreen(std::make_shared<BackgroundScreen>(), std::nullopt);
        screenManager_->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    }

    GameStateManagementGame::~GameStateManagementGame() = default;

    void GameStateManagementGame::LoadContent()
    {
        // ContentManager's statically typed C++ Load route requires the concrete runtime asset
        // type; retaining the texture has the same preload/cache effect as C# Load<object>.
        for (const auto& asset : preloadAssets_)
            gradientPreload_.emplace(getContentProperty().Load<Texture2D>(asset));
    }

    void GameStateManagementGame::Draw(const GameTime& gameTime)
    {
        graphics_->getGraphicsDeviceProperty()->Clear(Color::Black);
        Game::Draw(gameTime);
    }

    const std::string& GameStateManagementGame::GetTypeName() const
    {
        static const std::string name = "GameStateManagement.GameStateManagementGame";
        return name;
    }
}
