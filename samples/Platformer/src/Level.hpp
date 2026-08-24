// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Accelerometer.hpp"
#include "Enemy.hpp"
#include "Gem.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Content/ContentManager.hpp"
#include "Microsoft/Xna/Framework/DisplayOrientation.hpp"
#include "Microsoft/Xna/Framework/GameTime.hpp"
#include "Microsoft/Xna/Framework/Graphics/SpriteBatch.hpp"
#include "Microsoft/Xna/Framework/Graphics/Texture2D.hpp"
#include "Microsoft/Xna/Framework/Input/GamePadState.hpp"
#include "Microsoft/Xna/Framework/Input/KeyboardState.hpp"
#include "Microsoft/Xna/Framework/Input/Touch/TouchCollection.hpp"
#include "Microsoft/Xna/Framework/Point.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/IDisposable.hpp"
#include "System/IO/Stream.hpp"
#include "System/IServiceProvider.hpp"
#include "System/Random.hpp"
#include "System/TimeSpan.hpp"

namespace Platformer
{
    class Level final : public System::IDisposable
    {
    public:
        Level(System::IServiceProvider* serviceProvider, System::IO::Stream& fileStream,
              int levelIndex);
        ~Level() override = default;

        [[nodiscard]] Player* getPlayerProperty() const;
        [[nodiscard]] int getScoreProperty() const;
        [[nodiscard]] bool getReachedExitProperty() const;
        [[nodiscard]] System::TimeSpan getTimeRemainingProperty() const;
        [[nodiscard]] Microsoft::Xna::Framework::Content::ContentManager& getContentProperty();
        [[nodiscard]] int getWidthProperty() const;
        [[nodiscard]] int getHeightProperty() const;

        void Dispose() override;
        [[nodiscard]] TileCollision GetCollision(int x, int y) const;
        [[nodiscard]] Microsoft::Xna::Framework::Rectangle GetBounds(int x, int y) const;
        void Update(const Microsoft::Xna::Framework::GameTime& gameTime,
                    const Microsoft::Xna::Framework::Input::KeyboardState& keyboardState,
                    const Microsoft::Xna::Framework::Input::GamePadState& gamePadState,
                    const Microsoft::Xna::Framework::Input::Touch::TouchCollection& touchState,
                    const AccelerometerState& accelState,
                    Microsoft::Xna::Framework::DisplayOrientation orientation);
        void StartNewLife();
        void Draw(const Microsoft::Xna::Framework::GameTime& gameTime,
                  Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch);

    private:
        void LoadTiles(System::IO::Stream& fileStream);
        [[nodiscard]] Tile LoadTile(char tileType, int x, int y);
        [[nodiscard]] Tile LoadTile(const std::string& name, TileCollision collision);
        [[nodiscard]] Tile LoadVarietyTile(const std::string& baseName, int variationCount,
                                           TileCollision collision);
        [[nodiscard]] Tile LoadStartTile(int x, int y);
        [[nodiscard]] Tile LoadExitTile(int x, int y);
        [[nodiscard]] Tile LoadEnemyTile(int x, int y, const std::string& spriteSet);
        [[nodiscard]] Tile LoadGemTile(int x, int y);
        void UpdateGems(const Microsoft::Xna::Framework::GameTime& gameTime);
        void UpdateEnemies(const Microsoft::Xna::Framework::GameTime& gameTime);
        void OnGemCollected(Gem* gem, Player* collectedBy);
        void OnPlayerKilled(Enemy* killedBy);
        void OnExitReached();
        void DrawTiles(Microsoft::Xna::Framework::Graphics::SpriteBatch& spriteBatch);

        std::vector<std::vector<Tile>> tiles_;
        std::vector<Microsoft::Xna::Framework::Graphics::Texture2D> layers_;
        static constexpr int EntityLayer = 2;

        std::unique_ptr<Player> player_;
        std::vector<std::unique_ptr<Gem>> gems_;
        std::vector<std::unique_ptr<Enemy>> enemies_;

        Microsoft::Xna::Framework::Vector2 start_;
        Microsoft::Xna::Framework::Point exit_;
        inline static const Microsoft::Xna::Framework::Point InvalidPosition{-1, -1};

        System::Random random_{354668};
        int score_ = 0;
        bool reachedExit_ = false;
        System::TimeSpan timeRemaining_;
        static constexpr int PointsPerSecond = 5;

        Microsoft::Xna::Framework::Content::ContentManager content_;
        std::optional<Microsoft::Xna::Framework::Audio::SoundEffect> exitReachedSound_;
    };
}
