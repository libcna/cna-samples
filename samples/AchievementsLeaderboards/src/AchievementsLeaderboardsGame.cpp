// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AchievementsLeaderboardsGame.cs
//
// An XNA Game Studio 4.0 program written for CNA: no Microsoft sample covers
// achievements or leaderboards, which XNA reserved for licensed titles. It uses
// only the public XNA 4.0 API and compiles against the shipped assemblies.
//-----------------------------------------------------------------------------

#include "AchievementsLeaderboardsGame.hpp"

#include <algorithm>
#include <any>
#include <cmath>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameComponentCollection.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Achievement.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Gamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesComponent.hpp"
#include "Microsoft/Xna/Framework/GamerServices/GamerServicesNotAvailableException.hpp"
#include "Microsoft/Xna/Framework/GamerServices/Guide.hpp"
#include "Microsoft/Xna/Framework/GamerServices/LeaderboardEntry.hpp"
#include "Microsoft/Xna/Framework/GamerServices/LeaderboardKey.hpp"
#include "Microsoft/Xna/Framework/GamerServices/LeaderboardWriter.hpp"
#include "Microsoft/Xna/Framework/GamerServices/PropertyDictionary.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamer.hpp"
#include "Microsoft/Xna/Framework/GamerServices/SignedInGamerCollection.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Net/GameEndedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/GameStartedEventArgs.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkGamer.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionState.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "Microsoft/Xna/Framework/Net/WriteLeaderboardsEventArgs.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "System/Exception.hpp"
#include "System/IO/Stream.hpp"

namespace AchievementsLeaderboards
{
    using namespace Microsoft::Xna::Framework;
    using namespace Microsoft::Xna::Framework::GamerServices;
    using namespace Microsoft::Xna::Framework::Graphics;
    using namespace Microsoft::Xna::Framework::Input;
    using namespace Microsoft::Xna::Framework::Net;

    const LeaderboardIdentity AchievementsLeaderboardsGame::BestScore =
        LeaderboardIdentity::Create(LeaderboardKey::BestScoreLifeTime, 0);

    #pragma region Initialization

    AchievementsLeaderboardsGame::AchievementsLeaderboardsGame()
    {
        graphics = std::make_unique<GraphicsDeviceManager>(this);

        graphics->setPreferredBackBufferWidthProperty(1280);
        graphics->setPreferredBackBufferHeightProperty(720);

        getContentProperty().setRootDirectoryProperty("Content");

        gamerServices = std::make_unique<GamerServicesComponent>(*this);
        getComponentsProperty().Add(gamerServices.get());
    }

    AchievementsLeaderboardsGame::~AchievementsLeaderboardsGame()
    {
        // Releasing an outstanding operation cancels its callback, which would otherwise reach
        // a destroyed game.
        pending.reset();
        finished.reset();
        if (networkSession)
            networkSession->Dispose();
    }

    const std::string& AchievementsLeaderboardsGame::GetTypeName() const
    {
        static const std::string name = "AchievementsLeaderboards.AchievementsLeaderboardsGame";
        return name;
    }

    void AchievementsLeaderboardsGame::LoadContent()
    {
        spriteBatch.emplace(getGraphicsDeviceProperty());

        font.emplace(getContentProperty().Load<SpriteFont>("Font"));
    }

    void AchievementsLeaderboardsGame::UnloadContent()
    {
        for (auto& [key, picture] : pictures)
            picture.Dispose();

        if (networkSession)
            networkSession->Dispose();
    }

    #pragma endregion

    #pragma region Update

    void AchievementsLeaderboardsGame::Update(GameTime& gameTime)
    {
        finished.reset();

        HandleInput();

        if (!networkSession)
        {
            // Only listen for menu input when the Guide is not visible.
            if (!Guide::getIsVisibleProperty())
                UpdateMenu();
        }
        else
        {
            UpdateRound(gameTime);
        }

        Game::Update(gameTime);
    }

    void AchievementsLeaderboardsGame::UpdateMenu()
    {
        if (IsPressed(Keys::Escape, Buttons::Back))
        {
            Exit();
            return;
        }

        SignedInGamer* gamer = (*Gamer::getSignedInGamersProperty())[PlayerIndex::One];

        if (gamer == nullptr)
        {
            // Achievements and leaderboards belong to an online profile.
            if (IsPressed(Keys::Enter, Buttons::A))
            {
                try
                {
                    Guide::ShowSignIn(1, true);
                }
                catch (const GamerServicesNotAvailableException& error)
                {
                    message = error.getMessageProperty();
                }
            }

            return;
        }

        if (busy)
            return;

        if (!achievements)
        {
            Refresh(gamer);
        }
        else if (IsPressed(Keys::Enter, Buttons::A))
        {
            StartRound();
        }
        else if (IsPressed(Keys::Down, Buttons::DPadDown) &&
                 leaderboard && leaderboard->getCanPageDownProperty())
        {
            busy = true;
            pending.reset(leaderboard->BeginPageDown(
                [this](System::IAsyncResult& result) { PageFinished(result); }, false));
        }
        else if (IsPressed(Keys::Up, Buttons::DPadUp) &&
                 leaderboard && leaderboard->getCanPageUpProperty())
        {
            busy = true;
            pending.reset(leaderboard->BeginPageUp(
                [this](System::IAsyncResult& result) { PageFinished(result); }, true));
        }
    }

    void AchievementsLeaderboardsGame::UpdateRound(GameTime& gameTime)
    {
        networkSession->Update();

        if (roundOver)
        {
            // The leaderboard was written when the game ended; the session is done.
            roundOver = false;
            networkSession->Dispose();
            networkSession.reset();

            roundsPlayed++;
            message = "Round over: " + std::to_string(score) + " points.";

            SignedInGamer* gamer = (*Gamer::getSignedInGamersProperty())[PlayerIndex::One];

            if (gamer != nullptr)
            {
                awards.push("FirstRound");

                if (score >= HighScorePoints)
                    awards.push("HighScore");

                if (roundsPlayed >= VeteranRounds)
                    awards.push("Veteran");

                AwardNext(gamer);
            }
            return;
        }

        if (networkSession->getSessionStateProperty() != NetworkSessionState::Playing)
            return;

        if (IsPressed(Keys::Space, Buttons::A))
            score++;

        roundTimeLeft -= gameTime.getElapsedGameTimeProperty();

        if (roundTimeLeft <= System::TimeSpan::Zero && networkSession->getIsHostProperty())
            networkSession->EndGame();
    }

    void AchievementsLeaderboardsGame::StartRound()
    {
        busy = true;
        message = "Creating session...";

        pending.reset(NetworkSession::BeginCreate(NetworkSessionType::PlayerMatch, 1, 2,
            [this](System::IAsyncResult& result) { SessionCreated(result); }, {}));
    }

    void AchievementsLeaderboardsGame::SessionCreated(System::IAsyncResult& result)
    {
        Finished();

        try
        {
            networkSession.reset(NetworkSession::EndCreate(&result));

            networkSession->GameStarted += [this](System::Object*, const GameStartedEventArgs&) { GameStarted(); };
            networkSession->GameEnded += [this](System::Object*, const GameEndedEventArgs&) { GameEnded(); };
            networkSession->WriteUnarbitratedLeaderboard +=
                [this](System::Object*, const WriteLeaderboardsEventArgs& e) { WriteLeaderboard(e); };

            networkSession->StartGame();

            message.clear();
        }
        catch (const System::Exception& error)
        {
            message = error.getMessageProperty();
        }

        busy = false;
    }

    void AchievementsLeaderboardsGame::GameStarted()
    {
        score = 0;
        roundTimeLeft = System::TimeSpan::FromSeconds(RoundSeconds);
    }

    void AchievementsLeaderboardsGame::GameEnded()
    {
        roundOver = true;
    }

    void AchievementsLeaderboardsGame::WriteLeaderboard(const WriteLeaderboardsEventArgs& e)
    {
        LeaderboardEntry* entry = e.getGamerProperty()->getLeaderboardWriterProperty().GetLeaderboard(BestScore);

        entry->setRatingProperty(score);
        entry->getColumnsProperty().SetValue("Rounds", roundsPlayed + 1);
    }

    void AchievementsLeaderboardsGame::AwardNext(SignedInGamer* gamer)
    {
        busy = true;

        if (!awards.empty())
        {
            const std::string key = awards.front();
            awards.pop();
            pending.reset(gamer->BeginAwardAchievement(key,
                [this](System::IAsyncResult& result) { AwardFinished(result); }, gamer));
        }
        else
        {
            Refresh(gamer);
        }
    }

    void AchievementsLeaderboardsGame::AwardFinished(System::IAsyncResult& result)
    {
        Finished();

        auto* gamer = std::any_cast<SignedInGamer*>(result.getAsyncStateProperty());

        try
        {
            gamer->EndAwardAchievement(&result);
        }
        catch (const System::Exception& error)
        {
            message = error.getMessageProperty();
        }

        AwardNext(gamer);
    }

    void AchievementsLeaderboardsGame::Refresh(SignedInGamer* gamer)
    {
        busy = true;

        pending.reset(gamer->BeginGetAchievements(
            [this](System::IAsyncResult& result) { AchievementsRead(result); }, gamer));
    }

    void AchievementsLeaderboardsGame::AchievementsRead(System::IAsyncResult& result)
    {
        Finished();

        auto* gamer = std::any_cast<SignedInGamer*>(result.getAsyncStateProperty());

        try
        {
            AchievementCollection list = gamer->EndGetAchievements(&result);

            for (Achievement achievement : list)
            {
                if (!pictures.contains(achievement.getKeyProperty()))
                {
                    std::unique_ptr<System::IO::Stream> stream(achievement.GetPicture());
                    pictures.emplace(achievement.getKeyProperty(),
                                     Texture2D::FromStream(getGraphicsDeviceProperty(), *stream));
                }
            }

            if (achievements)
                achievements->Dispose();

            achievements.emplace(std::move(list));

            pending.reset(LeaderboardReader::BeginRead(BestScore, 0, PageSize,
                [this](System::IAsyncResult& read) { LeaderboardRead(read); }, {}));
        }
        catch (const System::Exception& error)
        {
            message = error.getMessageProperty();
            busy = false;
        }
    }

    void AchievementsLeaderboardsGame::LeaderboardRead(System::IAsyncResult& result)
    {
        Finished();

        try
        {
            LeaderboardReader reader = LeaderboardReader::EndRead(&result);

            if (leaderboard)
                leaderboard->Dispose();

            leaderboard.emplace(std::move(reader));
        }
        catch (const System::Exception& error)
        {
            message = error.getMessageProperty();
        }

        busy = false;
    }

    void AchievementsLeaderboardsGame::PageFinished(System::IAsyncResult& result)
    {
        Finished();

        try
        {
            if (std::any_cast<bool>(result.getAsyncStateProperty()))
                leaderboard->EndPageUp(&result);
            else
                leaderboard->EndPageDown(&result);
        }
        catch (const System::Exception& error)
        {
            message = error.getMessageProperty();
        }

        busy = false;
    }

    void AchievementsLeaderboardsGame::Finished()
    {
        finished = std::move(pending);
    }

    #pragma endregion

    #pragma region Draw

    void AchievementsLeaderboardsGame::Draw(const GameTime& gameTime)
    {
        getGraphicsDeviceProperty().Clear(Color::CornflowerBlue);

        spriteBatch->Begin();

        if (networkSession)
            DrawRound();
        else
            DrawMenu();

        spriteBatch->DrawString(*font, message, Vector2(60, 640), Color::White);

        spriteBatch->End();

        Game::Draw(gameTime);
    }

    void AchievementsLeaderboardsGame::DrawRound()
    {
        if (networkSession->getSessionStateProperty() != NetworkSessionState::Playing)
            return;

        const int seconds = std::max(0, static_cast<int>(std::ceil(roundTimeLeft.getTotalSecondsProperty())));
        const std::string text = "Press Space or A as often as you can!\n\n"
                                 "Score: " + std::to_string(score) + "\n"
                                 "Time: " + std::to_string(seconds);

        spriteBatch->DrawString(*font, text, Vector2(60, 60), Color::White);
    }

    void AchievementsLeaderboardsGame::DrawMenu()
    {
        SignedInGamer* gamer = (*Gamer::getSignedInGamersProperty())[PlayerIndex::One];

        if (gamer == nullptr)
        {
            spriteBatch->DrawString(*font, "Enter / A: sign in", Vector2(60, 60), Color::White);
            return;
        }

        spriteBatch->DrawString(*font, gamer->getGamertagProperty(), Vector2(60, 20), Color::Yellow);

        DrawAchievements(Vector2(60, 80));
        DrawLeaderboard(Vector2(700, 80));

        spriteBatch->DrawString(*font, "Enter / A: play a round   Up/Down: page   Esc / Back: exit",
                                Vector2(60, 680), Color::LightGray);
    }

    void AchievementsLeaderboardsGame::DrawAchievements(Vector2 position)
    {
        spriteBatch->DrawString(*font, "Achievements", position, Color::White);

        if (!achievements)
            return;

        position.Y += 50;

        for (const Achievement& achievement : *achievements)
        {
            const auto picture = pictures.find(achievement.getKeyProperty());

            if (picture != pictures.end())
            {
                const Color tint = achievement.getIsEarnedProperty() ? Color::White : Color::Gray;

                spriteBatch->Draw(picture->second,
                                  Rectangle(static_cast<int>(position.X), static_cast<int>(position.Y), 48, 48), tint);
            }

            std::string title, detail;

            if (achievement.getIsEarnedProperty())
            {
                title = achievement.getNameProperty() + " (" + std::to_string(achievement.getGamerScoreProperty()) + ")";
                detail = "Earned " + achievement.getEarnedDateTimeProperty().ToString("yyyy-MM-dd");
            }
            else if (achievement.getDisplayBeforeEarnedProperty())
            {
                title = achievement.getNameProperty() + " (" + std::to_string(achievement.getGamerScoreProperty()) + ")";
                detail = achievement.getHowToEarnProperty();
            }
            else
            {
                title = "Secret achievement";
                detail = "Keep playing";
            }

            spriteBatch->DrawString(*font, title, position + Vector2(60, 0), Color::White);
            spriteBatch->DrawString(*font, detail, position + Vector2(60, 24), Color::LightGray);

            position.Y += 70;
        }
    }

    void AchievementsLeaderboardsGame::DrawLeaderboard(Vector2 position)
    {
        if (!leaderboard)
            return;

        spriteBatch->DrawString(*font,
            "Best scores (" + std::to_string(leaderboard->getTotalLeaderboardSizeProperty()) + ")",
            position, Color::White);

        position.Y += 50;

        int rank = leaderboard->getPageStartProperty() + 1;

        for (const LeaderboardEntry& entry : leaderboard->getEntriesProperty())
        {
            const std::string text = std::to_string(rank) + ". " + entry.getGamerProperty()->getGamertagProperty() + "  " +
                                     std::to_string(entry.getRatingProperty()) +
                                     "  (round " + std::to_string(entry.getColumnsProperty().GetValueInt32("Rounds")) + ")";

            spriteBatch->DrawString(*font, text, position, Color::White);

            position.Y += 30;
            rank++;
        }
    }

    #pragma endregion

    #pragma region Handle Input

    void AchievementsLeaderboardsGame::HandleInput()
    {
        previousKeyboardState = currentKeyboardState;
        previousGamePadState = currentGamePadState;

        currentKeyboardState = Keyboard::GetState();
        currentGamePadState = GamePad::GetState(PlayerIndex::One);
    }

    bool AchievementsLeaderboardsGame::IsPressed(Keys key, Buttons button) const
    {
        return (currentKeyboardState.IsKeyDown(key) &&
                previousKeyboardState.IsKeyUp(key)) ||
               (currentGamePadState.IsButtonDown(button) &&
                previousGamePadState.IsButtonUp(button));
    }

    #pragma endregion
}
