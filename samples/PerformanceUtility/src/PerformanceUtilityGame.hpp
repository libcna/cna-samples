// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "CNA/CNAHelper.hpp"
#include "System/Diagnostics/Stopwatch.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"

#include "GameDebugTools/DebugSystem.hpp"

namespace System
{
    class IAsyncResult;
}

namespace PerformanceUtility
{
    /**
     * @brief Demonstrates the reusable GameDebugTools performance and command components.
     */
    class PerformanceUtilityGame final : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Creates the performance utility demonstration game. */
        PerformanceUtilityGame();

        /** @brief Releases the C++-owned debug components before the game graphics device. */
        ~PerformanceUtilityGame() override;

        /**
         * @brief Returns the fully qualified managed type name.
         *
         * @return The managed type name used by SharpRuntime type identity.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Initializes the debug system, command, and touch gestures. */
        void Initialize() override;

        /** @brief Loads the original sprite font and cat texture. */
        void LoadContent() override;

        /**
         * @brief Updates the profiling marks, controls, and debug components.
         *
         * @param gameTime Timing information for the current update.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Draws the usage board, cat, and debug components.
         *
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        void PosCommand(
            GameDebugTools::IDebugCommandHost& host,
            const std::string& command,
            const std::vector<std::string>& arguments);
        void HandleInput();
        [[nodiscard]] bool IsButtonOrKeyPressed(
            Microsoft::Xna::Framework::Input::Buttons button,
            Microsoft::Xna::Framework::Input::Keys key) const;
        void HandleTouchInput();
        void InputDebugCommandCallback(System::IAsyncResult& result);

        Microsoft::Xna::Framework::GraphicsDeviceManager graphics_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch_;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> blank_;
        std::optional<Microsoft::Xna::Framework::Graphics::Texture2D> cat_;

        GameDebugTools::DebugSystem* debugSystem_ = nullptr;
        Microsoft::Xna::Framework::Vector2 debugPos_{100.0f, 100.0f};
        System::Diagnostics::Stopwatch stopwatch_;

        Microsoft::Xna::Framework::Input::GamePadState padState_;
        Microsoft::Xna::Framework::Input::KeyboardState keyState_;
        Microsoft::Xna::Framework::Input::GamePadState prevPadState_;
        Microsoft::Xna::Framework::Input::KeyboardState prevKeyState_;
    };
}
