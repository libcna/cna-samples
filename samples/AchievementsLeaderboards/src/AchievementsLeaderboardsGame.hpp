// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AchievementsLeaderboardsGame.cs
//
// An XNA Game Studio 4.0 program written for CNA: no Microsoft sample covers
// achievements or leaderboards, which XNA reserved for licensed titles. It uses
// only the public XNA 4.0 API and compiles against the shipped assemblies.
//-----------------------------------------------------------------------------
#pragma once

#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GamerServices/AchievementCollection.hpp"
#include "Microsoft/Xna/Framework/GamerServices/LeaderboardIdentity.hpp"
#include "Microsoft/Xna/Framework/GamerServices/LeaderboardReader.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Input/Buttons.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSession.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/IAsyncResult.hpp"
#include "System/TimeSpan.hpp"

namespace Microsoft::Xna::Framework::GamerServices
{
    class GamerServicesComponent;
    class SignedInGamer;
}

namespace Microsoft::Xna::Framework::Net
{
    class WriteLeaderboardsEventArgs;
}

namespace AchievementsLeaderboards
{
    /**
     * @brief Plays short rounds in a PlayerMatch session. When a round ends the score is written
     * to a leaderboard, achievements are awarded, and both lists are read back and drawn.
     */
    class AchievementsLeaderboardsGame final : public Microsoft::Xna::Framework::Game
    {
    public:
        /** @brief Creates the graphics device manager and the gamer services component. */
        AchievementsLeaderboardsGame();

        /** @brief Releases any session and asynchronous operation still outstanding. */
        ~AchievementsLeaderboardsGame() override;

        /**
         * @brief Returns the fully qualified managed type name.
         *
         * @return The managed type name used by SharpRuntime type identity.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /** @brief Loads your content. */
        void LoadContent() override;

        /** @brief Releases the pictures and any session still open. */
        void UnloadContent() override;

        /**
         * @brief Allows the game to run logic.
         *
         * @param gameTime Timing information for the current update.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief This is called when the game should draw itself.
         *
         * @param gameTime Timing information for the current frame.
         */
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime) override;

    private:
        static constexpr int RoundSeconds = 5;
        static constexpr int HighScorePoints = 20;
        static constexpr int VeteranRounds = 3;
        static constexpr int PageSize = 8;

        static const Microsoft::Xna::Framework::GamerServices::LeaderboardIdentity BestScore;

        void UpdateMenu();
        void UpdateRound(Microsoft::Xna::Framework::GameTime& gameTime);
        void StartRound();
        void SessionCreated(System::IAsyncResult& result);
        void GameStarted();
        void GameEnded();
        void WriteLeaderboard(const Microsoft::Xna::Framework::Net::WriteLeaderboardsEventArgs& e);
        void AwardNext(Microsoft::Xna::Framework::GamerServices::SignedInGamer* gamer);
        void AwardFinished(System::IAsyncResult& result);
        void Refresh(Microsoft::Xna::Framework::GamerServices::SignedInGamer* gamer);
        void AchievementsRead(System::IAsyncResult& result);
        void LeaderboardRead(System::IAsyncResult& result);
        void PageFinished(System::IAsyncResult& result);

        /**
         * @brief Takes ownership of the finished operation, which the garbage collector released
         * in C#; it is destroyed at the next Update, outside the call that completed it.
         */
        void Finished();

        void DrawRound();
        void DrawMenu();
        void DrawAchievements(Microsoft::Xna::Framework::Vector2 position);
        void DrawLeaderboard(Microsoft::Xna::Framework::Vector2 position);

        void HandleInput();
        [[nodiscard]] bool IsPressed(Microsoft::Xna::Framework::Input::Keys key,
                                     Microsoft::Xna::Framework::Input::Buttons button) const;

        std::unique_ptr<Microsoft::Xna::Framework::GraphicsDeviceManager> graphics;
        std::unique_ptr<Microsoft::Xna::Framework::GamerServices::GamerServicesComponent> gamerServices;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteBatch> spriteBatch;
        std::optional<Microsoft::Xna::Framework::Graphics::SpriteFont> font;

        std::unique_ptr<Microsoft::Xna::Framework::Net::NetworkSession> networkSession;
        bool roundOver = false;
        int score = 0;
        int roundsPlayed = 0;
        System::TimeSpan roundTimeLeft;

        std::optional<Microsoft::Xna::Framework::GamerServices::AchievementCollection> achievements;
        std::map<std::string, Microsoft::Xna::Framework::Graphics::Texture2D> pictures;
        std::optional<Microsoft::Xna::Framework::GamerServices::LeaderboardReader> leaderboard;
        std::queue<std::string> awards;

        // True while an asynchronous operation is outstanding.
        bool busy = false;
        std::string message;

        // The outstanding operation, and the last one to finish.
        std::unique_ptr<System::IAsyncResult> pending;
        std::unique_ptr<System::IAsyncResult> finished;

        Microsoft::Xna::Framework::Input::KeyboardState currentKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState currentGamePadState;
        Microsoft::Xna::Framework::Input::KeyboardState previousKeyboardState;
        Microsoft::Xna::Framework::Input::GamePadState previousGamePadState;
    };
}
