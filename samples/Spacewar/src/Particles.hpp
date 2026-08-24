// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>
#include <string>

#include "SceneItem.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Random.hpp"

namespace Microsoft::Xna::Framework::Graphics
{
    class SpriteBatch;
}

namespace Spacewar
{
    class Particles final : public SceneItem
    {
    public:
        explicit Particles(Microsoft::Xna::Framework::Game* game);
        static void Dispose();
        void Render() override;
        void AddShipTrail(Microsoft::Xna::Framework::Matrix world,
                          Microsoft::Xna::Framework::Vector2 direction);
        void AddRocketTrail(Microsoft::Xna::Framework::Matrix world,
                            Microsoft::Xna::Framework::Vector2 direction);
        void AddExplosion(Microsoft::Xna::Framework::Vector3 position);
        void OnCreateDevice() override;

    private:
        inline static const std::string particleTexture_ = "textures/circle";
        static System::Random random_;
        static std::unique_ptr<Microsoft::Xna::Framework::Graphics::SpriteBatch> batch_;
    };
}
