// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <memory>
#include <optional>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"

namespace GameStateManagement
{
    class ScreenManager;

    /**
     * @brief Hosts the screen-manager demonstration and its initial screen stack.
     */
    class GameStateManagementGame final : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Constructs the game, graphics manager and initial screens. */
        GameStateManagementGame();
        /** @brief Destroys the screen manager after its complete type is available. */
        ~GameStateManagementGame() override;
        /** @brief Gets the fully qualified logical type name. @return Type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Preloads the transition asset used by the user interface. */
        void LoadContent() override;
        /** @brief Clears the frame and delegates screen drawing to components. @param gameTime Timing. */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics_;
        std::unique_ptr<ScreenManager> screenManager_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> gradientPreload_;
        static const std::array<std::string, 1> preloadAssets_;
    };
}
