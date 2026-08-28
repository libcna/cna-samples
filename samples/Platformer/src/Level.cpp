// SPDX-License-Identifier: MS-PL

#include "Level.hpp"

#include <algorithm>
#include <cmath>
#include <utility>

#include "RectangleExtensions.hpp"
#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "System/Exception.hpp"
#include "System/IO/StreamReader.hpp"
#include "System/NotSupportedException.hpp"
#include "System/Int32.hpp"

namespace Platformer
{
    Level::Level(System::IServiceProvider* serviceProvider, System::IO::Stream& fileStream,
                 const int levelIndex)
        : exit_(InvalidPosition),
          timeRemaining_(System::TimeSpan::FromMinutes(2.0)),
          content_(serviceProvider, "Content")
    {
        LoadTiles(fileStream);

        layers_.reserve(3);
        for (int i = 0; i < 3; ++i)
        {
            layers_.push_back(content_.Load<Microsoft::Xna::Framework::Graphics::Texture2D>(
                "Backgrounds/Layer" + System::Int32::ToString(i) + "_" +
                System::Int32::ToString(levelIndex)));
        }

        exitReachedSound_.emplace(
            content_.Load<Microsoft::Xna::Framework::Audio::SoundEffect>("Sounds/ExitReached"));
    }

    Player* Level::getPlayerProperty() const
    {
        return player_.get();
    }

    int Level::getScoreProperty() const
    {
        return score_;
    }

    bool Level::getReachedExitProperty() const
    {
        return reachedExit_;
    }

    System::TimeSpan Level::getTimeRemainingProperty() const
    {
        return timeRemaining_;
    }

    Microsoft::Xna::Framework::Content::ContentManager& Level::getContentProperty()
    {
        return content_;
    }

    int Level::getWidthProperty() const
    {
        return static_cast<int>(tiles_.size());
    }

    int Level::getHeightProperty() const
    {
        return tiles_.empty() ? 0 : static_cast<int>(tiles_.front().size());
    }

    void Level::Dispose()
    {
        content_.Unload();
    }

    void Level::LoadTiles(System::IO::Stream& fileStream)
    {
        std::vector<std::string> lines;
        int width = 0;

        {
            System::IO::StreamReader reader(&fileStream);
            if (reader.Peek() < 0)
                throw System::Exception("The level file is empty.");

            std::string line = reader.ReadLine();
            width = static_cast<int>(line.length());
            while (true)
            {
                lines.push_back(line);
                if (static_cast<int>(line.length()) != width)
                    throw System::Exception(
                        "The length of line " + System::Int32::ToString(static_cast<int>(lines.size())) +
                        " is different from all preceeding lines.");
                if (reader.Peek() < 0)
                    break;
                line = reader.ReadLine();
            }
        }

        tiles_.resize(static_cast<std::size_t>(width),
                      std::vector<Tile>(lines.size()));
        for (int y = 0; y < getHeightProperty(); ++y)
            for (int x = 0; x < getWidthProperty(); ++x)
                tiles_[static_cast<std::size_t>(x)][static_cast<std::size_t>(y)] =
                    LoadTile(lines[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)], x, y);

        if (player_ == nullptr)
            throw System::NotSupportedException("A level must have a starting point.");
        if (exit_ == InvalidPosition)
            throw System::NotSupportedException("A level must have an exit.");
    }

    Tile Level::LoadTile(const char tileType, const int x, const int y)
    {
        switch (tileType)
        {
            case '.':
                return {std::nullopt, TileCollision::Passable};
            case 'X':
                return LoadExitTile(x, y);
            case 'G':
                return LoadGemTile(x, y);
            case '-':
                return LoadTile("Platform", TileCollision::Platform);
            case 'A':
                return LoadEnemyTile(x, y, "MonsterA");
            case 'B':
                return LoadEnemyTile(x, y, "MonsterB");
            case 'C':
                return LoadEnemyTile(x, y, "MonsterC");
            case 'D':
                return LoadEnemyTile(x, y, "MonsterD");
            case '~':
                return LoadVarietyTile("BlockB", 2, TileCollision::Platform);
            case ':':
                return LoadVarietyTile("BlockB", 2, TileCollision::Passable);
            case '1':
                return LoadStartTile(x, y);
            case '#':
                return LoadVarietyTile("BlockA", 7, TileCollision::Impassable);
            default:
                throw System::NotSupportedException(
                    "Unsupported tile type character '" + std::string(1, tileType) +
                    "' at position " + System::Int32::ToString(x) + ", " + System::Int32::ToString(y) + ".");
        }
    }

    Tile Level::LoadTile(const std::string& name, const TileCollision collision)
    {
        return {content_.Load<Microsoft::Xna::Framework::Graphics::Texture2D>("Tiles/" + name),
                collision};
    }

    Tile Level::LoadVarietyTile(const std::string& baseName, const int variationCount,
                                const TileCollision collision)
    {
        return LoadTile(baseName + System::Int32::ToString(random_.Next(variationCount)), collision);
    }

    Tile Level::LoadStartTile(const int x, const int y)
    {
        if (player_ != nullptr)
            throw System::NotSupportedException("A level may only have one starting point.");

        start_ = RectangleExtensions::GetBottomCenter(GetBounds(x, y));
        player_ = std::make_unique<Player>(this, start_);
        return {std::nullopt, TileCollision::Passable};
    }

    Tile Level::LoadExitTile(const int x, const int y)
    {
        if (exit_ != InvalidPosition)
            throw System::NotSupportedException("A level may only have one exit.");

        exit_ = GetBounds(x, y).getCenterProperty();
        return LoadTile("Exit", TileCollision::Passable);
    }

    Tile Level::LoadEnemyTile(const int x, const int y, const std::string& spriteSet)
    {
        enemies_.push_back(std::make_unique<Enemy>(
            this, RectangleExtensions::GetBottomCenter(GetBounds(x, y)), spriteSet));
        return {std::nullopt, TileCollision::Passable};
    }

    Tile Level::LoadGemTile(const int x, const int y)
    {
        const Microsoft::Xna::Framework::Point position = GetBounds(x, y).getCenterProperty();
        gems_.push_back(std::make_unique<Gem>(
            this, Microsoft::Xna::Framework::Vector2(
                      static_cast<float>(position.X), static_cast<float>(position.Y))));
        return {std::nullopt, TileCollision::Passable};
    }

    TileCollision Level::GetCollision(const int x, const int y) const
    {
        if (x < 0 || x >= getWidthProperty())
            return TileCollision::Impassable;
        if (y < 0 || y >= getHeightProperty())
            return TileCollision::Passable;
        return tiles_[static_cast<std::size_t>(x)][static_cast<std::size_t>(y)].Collision;
    }

    Microsoft::Xna::Framework::Rectangle Level::GetBounds(const int x, const int y) const
    {
        return {x * Tile::Width, y * Tile::Height, Tile::Width, Tile::Height};
    }

    void Level::Update(
        const Microsoft::Xna::Framework::GameTime& gameTime,
        const Microsoft::Xna::Framework::Input::KeyboardState& keyboardState,
        const Microsoft::Xna::Framework::Input::GamePadState& gamePadState,
        const Microsoft::Xna::Framework::Input::Touch::TouchCollection& touchState,
        const AccelerometerState& accelState,
        const Microsoft::Xna::Framework::DisplayOrientation orientation)
    {
        if (!player_->getIsAliveProperty() || timeRemaining_ == System::TimeSpan::Zero)
        {
            player_->ApplyPhysics(gameTime);
        }
        else if (reachedExit_)
        {
            int seconds = static_cast<int>(std::round(
                gameTime.getElapsedGameTimeProperty().getTotalSecondsProperty() * 100.0));
            seconds = std::min(
                seconds, static_cast<int>(std::ceil(timeRemaining_.getTotalSecondsProperty())));
            timeRemaining_ = timeRemaining_ - System::TimeSpan::FromSeconds(seconds);
            score_ += seconds * PointsPerSecond;
        }
        else
        {
            timeRemaining_ = timeRemaining_ - gameTime.getElapsedGameTimeProperty();
            player_->Update(gameTime, keyboardState, gamePadState, touchState, accelState,
                            orientation);
            UpdateGems(gameTime);

            if (player_->getBoundingRectangleProperty().getTopProperty() >=
                getHeightProperty() * Tile::Height)
                OnPlayerKilled(nullptr);

            UpdateEnemies(gameTime);

            if (player_->getIsAliveProperty() && player_->getIsOnGroundProperty() &&
                player_->getBoundingRectangleProperty().Contains(exit_))
                OnExitReached();
        }

        if (timeRemaining_ < System::TimeSpan::Zero)
            timeRemaining_ = System::TimeSpan::Zero;
    }

    void Level::UpdateGems(const Microsoft::Xna::Framework::GameTime& gameTime)
    {
        for (std::size_t i = 0; i < gems_.size();)
        {
            gems_[i]->Update(gameTime);
            if (gems_[i]->getBoundingCircleProperty().Intersects(
                    player_->getBoundingRectangleProperty()))
            {
                std::unique_ptr<Gem> gem = std::move(gems_[i]);
                gems_.erase(gems_.begin() + static_cast<std::ptrdiff_t>(i));
                OnGemCollected(gem.get(), player_.get());
            }
            else
            {
                ++i;
            }
        }
    }

    void Level::UpdateEnemies(const Microsoft::Xna::Framework::GameTime& gameTime)
    {
        for (auto& enemy : enemies_)
        {
            enemy->Update(gameTime);
            if (enemy->getBoundingRectangleProperty().Intersects(
                    player_->getBoundingRectangleProperty()))
                OnPlayerKilled(enemy.get());
        }
    }

    void Level::OnGemCollected(Gem* gem, Player* collectedBy)
    {
        score_ += Gem::PointValue;
        gem->OnCollected(collectedBy);
    }

    void Level::OnPlayerKilled(Enemy* killedBy)
    {
        player_->OnKilled(killedBy);
    }

    void Level::OnExitReached()
    {
        player_->OnReachedExit();
        exitReachedSound_->Play();
        reachedExit_ = true;
    }

    void Level::StartNewLife()
    {
        player_->Reset(start_);
    }

    void Level::Draw(const Microsoft::Xna::Framework::GameTime& gameTime,
                     Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch)
    {
        using namespace Microsoft::Xna::Framework;

        for (int i = 0; i <= EntityLayer; ++i)
            spriteBatch.Draw(layers_[static_cast<std::size_t>(i)], Vector2::Zero, Color::White);

        DrawTiles(spriteBatch);
        for (auto& gem : gems_)
            gem->Draw(gameTime, spriteBatch);
        player_->Draw(gameTime, spriteBatch);
        for (auto& enemy : enemies_)
            enemy->Draw(gameTime, spriteBatch);
        for (int i = EntityLayer + 1; i < static_cast<int>(layers_.size()); ++i)
            spriteBatch.Draw(layers_[static_cast<std::size_t>(i)], Vector2::Zero, Color::White);
    }

    void Level::DrawTiles(Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch)
    {
        for (int y = 0; y < getHeightProperty(); ++y)
        {
            for (int x = 0; x < getWidthProperty(); ++x)
            {
                auto& texture =
                    tiles_[static_cast<std::size_t>(x)][static_cast<std::size_t>(y)].Texture;
                if (texture.has_value())
                {
                    const Microsoft::Xna::Framework::Vector2 position =
                        Microsoft::Xna::Framework::Vector2(
                            static_cast<float>(x), static_cast<float>(y)) *
                        Tile::Size;
                    spriteBatch.Draw(*texture, position, Microsoft::Xna::Framework::Color::White);
                }
            }
        }
    }
}
