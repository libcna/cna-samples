// SPDX-License-Identifier: MS-PL

#pragma once

namespace Microsoft::Xna::Framework
{
    class GameTime;
}

namespace RacingGame::GameScreens
{
    /** @brief Identifies one original Racing game-screen state. */
    enum class ScreenKind
    {
        Loading,
        Splash,
        MainMenu,
        CarSelection,
        TrackSelection,
        Game,
        Highscores,
        Options,
        Help,
    };

    /** @brief Common interface used by the original Racing screen stack. */
    class IGameScreen
    {
    public:
        /** @brief Destroys a screen through the common interface. */
        virtual ~IGameScreen() = default;
        /** @brief Runs per-frame screen logic before rendering. */
        virtual void Update(Microsoft::Xna::Framework::GameTime& gameTime) = 0;
        /** @brief Renders the screen and returns true when it should be popped. */
        [[nodiscard]] virtual bool Render() = 0;
        /** @brief Gets the concrete screen identity without runtime reflection. */
        [[nodiscard]] virtual ScreenKind getKindProperty() const = 0;
    };
}
