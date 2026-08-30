// SPDX-License-Identifier: MS-PL
#pragma once

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "System/TimeSpan.hpp"
#include "System/IO/FileMode.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFile.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFileStream.hpp"
#include "System/IO/StreamReader.hpp"
#include "System/IO/StreamWriter.hpp"

#include "../ScreenManager/GameScreen.hpp"
#include "../ScreenManager/ScreenManager.hpp"
#include "System/String.hpp"
#include "System/Int32.hpp"

namespace MarbleMazeGame {

using GameStateManagement::GameScreen;
using GameStateManagement::InputState;
using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::GameTime;
using Microsoft::Xna::Framework::Vector2;
using Microsoft::Xna::Framework::Graphics::SpriteBatch;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Input::Touch::GestureType;
using System::TimeSpan;

class HighScoreScreen : public GameScreen {
public:
    static constexpr int HighscorePlaces = 10;

    struct Entry {
        std::string Name;
        TimeSpan Value;
    };

    static std::vector<Entry>& HighScores() { return highScore_; }

    HighScoreScreen() { setEnabledGestures(GestureType::Tap); }

    void LoadContent() override {
        highScoreFont_ = Load<SpriteFont>("Fonts/MenuFont");
        GameScreen::LoadContent();
    }

    void HandleInput(InputState& input) override;

    void Draw(const GameTime& gameTime) override {
        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();

        spriteBatch.Begin();

        spriteBatch.DrawString(*highScoreFont_, "High Scores", Vector2(30, 30), Color::White);

        for (std::size_t i = 0; i < highScore_.size(); i++) {
            spriteBatch.DrawString(*highScoreFont_, System::Int32::ToString(static_cast<int>(i + 1)) + ". " + highScore_[i].Name,
                                    Vector2(100, (float)i * 40 + 70), Color::YellowGreen);

            int minutes = (int)highScore_[i].Value.getMinutesProperty();
            int seconds = (int)highScore_[i].Value.getSecondsProperty();
            // String.Format("{0:00}:{1:00}", highScore[i].Value.Minutes, ...Seconds)
            const std::string time = System::String::Format("{0:00}:{1:00}", minutes, seconds);
            spriteBatch.DrawString(*highScoreFont_, time, Vector2(500, (float)i * 40 + 70), Color::YellowGreen);
        }

        spriteBatch.End();
        GameScreen::Draw(gameTime);
    }

    // Check if a score belongs on the high score table.
    static bool IsInHighscores(TimeSpan gameTime) { return gameTime < highScore_[HighscorePlaces - 1].Value; }

    // Put high score on the highscores table.
    static void PutHighScore(const std::string& playerName, TimeSpan gameTime) {
        if (IsInHighscores(gameTime)) {
            highScore_[HighscorePlaces - 1] = Entry{playerName, gameTime};
            OrderGameScore();
        }
    }

    static void SaveHighscore() {
        auto isf = System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();
        System::IO::IsolatedStorage::IsolatedStorageFileStream isfs(
            "highscores.txt", System::IO::FileMode::Create, isf);
        System::IO::StreamWriter writer(&isfs, true);
        for (const auto& entry : highScore_) {
            writer.WriteLine(entry.Name);
            writer.WriteLine(entry.Value.ToString());
        }
        writer.Flush();
    }

    static void LoadHighscore() {
        auto isf = System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();
        if (isf.FileExists("highscores.txt")) {
            System::IO::IsolatedStorage::IsolatedStorageFileStream isfs(
                "highscores.txt", System::IO::FileMode::Open, isf);
            System::IO::StreamReader reader(&isfs, true);
            std::size_t i = 0;
            while (reader.Peek() != -1) {
                const std::string name = reader.ReadLine();
                const std::string score = reader.ReadLine();
                highScore_.at(i++) = Entry{name, TimeSpan::Parse(score)};
            }
        }
        OrderGameScore();
    }

private:
    void Exit();

    static void OrderGameScore() {
        std::sort(highScore_.begin(), highScore_.end(),
                  [](const Entry& a, const Entry& b) { return a.Value.getTicksProperty() < b.Value.getTicksProperty(); });
    }

    static inline std::vector<Entry> highScore_ = {
        {"Jasper", TimeSpan::FromSeconds(90)},  {"Ellen", TimeSpan::FromSeconds(110)},
        {"Terry", TimeSpan::FromSeconds(130)},  {"Lori", TimeSpan::FromSeconds(150)},
        {"Michael", TimeSpan::FromSeconds(170)}, {"Carol", TimeSpan::FromSeconds(190)},
        {"Toni", TimeSpan::FromSeconds(210)},   {"Cassie", TimeSpan::FromSeconds(230)},
        {"Luca", TimeSpan::FromSeconds(250)},   {"Brian", TimeSpan::FromSeconds(270)},
    };

    std::optional<SpriteFont> highScoreFont_;
};

} // namespace MarbleMazeGame
