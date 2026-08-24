// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>

#include "Enums.hpp"
#include "SceneItem.hpp"
#include "System/TimeSpan.hpp"

namespace Microsoft::Xna::Framework
{
    class Game;
    namespace Graphics
    {
        class SpriteBatch;
    }
}

namespace Spacewar
{
    class Screen
    {
    public:
        explicit Screen(Microsoft::Xna::Framework::Game* game);
        virtual ~Screen();

        [[nodiscard]] Microsoft::Xna::Framework::Game* getGameInstanceProperty() const;
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::SpriteBatch* getSpriteBatchProperty() const;
        virtual GameState Update(System::TimeSpan time, System::TimeSpan elapsedTime);
        virtual void Render();
        virtual void Shutdown();
        virtual void OnCreateDevice();

    protected:
        std::unique_ptr<SceneItem> scene_;
        std::unique_ptr<Screen> overlay_;
        std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> batch_;
        Microsoft::Xna::Framework::Game* game_;
    };
}
