// SPDX-License-Identifier: MS-PL
#pragma once

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteFont.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/GestureType.hpp"
#include "System/Collections/Generic/Dictionary.hpp"
#include "System/Int32.hpp"
#include "System/InvalidOperationException.hpp"
#include "System/IO/FileMode.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFile.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFileStream.hpp"
#include "System/IO/StreamReader.hpp"
#include "System/IO/StreamWriter.hpp"

#include "../GameConstants.hpp"
#include "../ScreenManager/GameScreen.hpp"
#include "BackgroundScreen.hpp"
#include "MainMenuScreen.hpp"

namespace NinjAcademy {

using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Graphics::SpriteFont;
using Microsoft::Xna::Framework::Graphics::Texture2D;

class HighScoreScreen : public GameScreen {
public:
    static const int HighscorePlaces = 7;

    static std::vector<std::pair<std::string, int>>& HighScore() {
        static std::vector<std::pair<std::string, int>> highScore = {
            {"Goku", 9001}, {"Ellen", 500}, {"Terry", 250}, {"Dave", 100},
            {"Biff", 50}, {"Michael", 20}, {"Dan Hibiki", 10},
        };
        return highScore;
    }

    static bool& HighscoreLoaded() {
        static bool loaded = false;
        return loaded;
    }

    static bool& HighscoreSaved() {
        static bool saved = false;
        return saved;
    }

    HighScoreScreen() {
        setEnabledGestures(GestureType::Tap);
        if (!HighscoreLoaded())
            throw System::InvalidOperationException("Missing highscore data");

        InitializeMapping();
    }

    void LoadContent() override {
        highScoreFont_.emplace(Load<SpriteFont>("Fonts/HighScoreFont"));
        highScoreTitleTexture_.emplace(Load<Texture2D>("Textures/highscore_title"));
        textShadowVector_ = Vector2(4.0f, 4.0f);
        viewport_ = GetScreenManager()->getGraphicsDeviceProperty().getViewportProperty().getBoundsProperty();
        titlePosition_ = Vector2(
            (float)(viewport_.getCenterProperty().X - highScoreTitleTexture_->getWidthProperty() / 2),
            (float)GameConstants::HighScoreTitleTopMargin);
        GameScreen::LoadContent();
    }

    void HandleInput(InputState& input) override {
        if (input.IsPauseGame(std::nullopt))
            Exit();

        if (!input.Gestures.empty() && input.Gestures[0].getGestureTypeProperty() == GestureType::Tap) {
            Exit();
            input.Gestures.clear();
        }
    }

    void Draw(const GameTime& gameTime) override {
        GameScreen::Draw(gameTime);
        if (!HighscoreLoaded())
            return;

        SpriteBatch& spriteBatch = GetScreenManager()->getSpriteBatch();
        spriteBatch.Begin();

        spriteBatch.Draw(*highScoreTitleTexture_, titlePosition_, Color::White);
        auto& highScore = HighScore();
        for (size_t i = 0; i < highScore.size(); i++) {
            if (highScore[i].first.empty())
                continue;

            Vector2 textPosition(
                (float)GameConstants::HighScorePlaceLeftMargin,
                (float)(i * GameConstants::HighScoreVerticalJump + GameConstants::HighScoreTopMargin));
            spriteBatch.DrawString(*highScoreFont_, GetPlaceString((int)i),
                                   textPosition + textShadowVector_, Color::Black);
            spriteBatch.DrawString(*highScoreFont_, GetPlaceString((int)i), textPosition, Color::White);

            textPosition.X = (float)GameConstants::HighScoreNameLeftMargin;
            spriteBatch.DrawString(*highScoreFont_, highScore[i].first,
                                   textPosition + textShadowVector_, Color::Black);
            spriteBatch.DrawString(*highScoreFont_, highScore[i].first, textPosition, Color::White);

            textPosition.X = (float)GameConstants::HighScoreScoreLeftMargin;
            const std::string score = System::Int32::ToString(highScore[i].second);
            spriteBatch.DrawString(*highScoreFont_, score, textPosition + textShadowVector_, Color::Black);
            spriteBatch.DrawString(*highScoreFont_, score, textPosition, Color::White);
        }
        spriteBatch.End();
    }

    static bool IsInHighscores(int score) { return score > HighScore()[HighscorePlaces - 1].second; }

    static void PutHighScore(const std::string& playerName, int score) {
        if (IsInHighscores(score)) {
            HighScore()[HighscorePlaces - 1] = {playerName, score};
            OrderGameScore();
            SaveHighscore();
        }
    }

    static void HighScoreChanged() { HighscoreSaved() = false; }

    static void SaveHighscore() {
        auto storage = System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();
        auto stream = storage.CreateFile(HighScoreFilename());
        System::IO::StreamWriter writer(&stream, true);
        for (const auto& entry : HighScore()) {
            writer.WriteLine(entry.first);
            writer.WriteLine(System::Int32::ToString(entry.second));
        }
        writer.Flush();
        HighscoreSaved() = true;
    }

    static void LoadHighscores() {
        auto storage = System::IO::IsolatedStorage::IsolatedStorageFile::GetUserStoreForApplication();
        if (storage.FileExists(HighScoreFilename())) {
            auto stream = storage.OpenFile(HighScoreFilename(), System::IO::FileMode::Open);
            System::IO::StreamReader reader(&stream, true);
            auto& highScore = HighScore();
            size_t i = 0;
            while (reader.Peek() != -1) {
                const std::string name = reader.ReadLine();
                const std::string score = reader.ReadLine();
                highScore.at(i++) = {name, System::Int32::Parse(score)};
            }
        }
        OrderGameScore();
        HighscoreLoaded() = true;
    }

private:
    static const std::string& HighScoreFilename() {
        static const std::string name = "highscores.txt";
        return name;
    }

    static void OrderGameScore() {
        auto& highScore = HighScore();
        /// List<T>.Sort falls back to insertion sort below 16 elements, which is
        /// stable, so a seven-entry table orders ties the same way here.
        std::stable_sort(highScore.begin(), highScore.end(),
                         [](const auto& left, const auto& right) { return CompareScores(left, right) < 0; });
    }

    // Comparison method used to compare two high-score entries. Returns 1 if the
    // first high-score is smaller than the second, 0 if both are equal and -1
    // otherwise.
    static int CompareScores(const std::pair<std::string, int>& score1,
                             const std::pair<std::string, int>& score2) {
        if (score1.second < score2.second)
            return 1;

        if (score1.second == score2.second)
            return 0;

        return -1;
    }

    void Exit() {
        ExitScreen();
        GetScreenManager()->AddScreen(std::make_shared<BackgroundScreen>("titlescreenBG"), std::nullopt);
        GetScreenManager()->AddScreen(std::make_shared<MainMenuScreen>(), std::nullopt);
    }

    // Gets a string describing an index's position in the high-score.
    std::string GetPlaceString(int number) const { return numberPlaceMapping_[number]; }

    // Initializes the mapping between score indices and position strings.
    void InitializeMapping() {
        numberPlaceMapping_.Add(0, "1.");
        numberPlaceMapping_.Add(1, "2.");
        numberPlaceMapping_.Add(2, "3.");
        numberPlaceMapping_.Add(3, "4.");
        numberPlaceMapping_.Add(4, "5.");
        numberPlaceMapping_.Add(5, "6.");
        numberPlaceMapping_.Add(6, "7.");
    }

    System::Collections::Generic::Dictionary<int, std::string> numberPlaceMapping_;

    std::optional<SpriteFont> highScoreFont_;
    std::optional<Texture2D> highScoreTitleTexture_;
    Vector2 titlePosition_;
    Vector2 textShadowVector_;
    Rectangle viewport_;
};

} // namespace NinjAcademy
