// SPDX-License-Identifier: MS-PL
#pragma once

#include "SceneItem.hpp"
#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/Vector4.hpp"

namespace Spacewar
{
    class Particle final : public SceneItem
    {
    public:
        Particle(Microsoft::Xna::Framework::Game* game,
                 Microsoft::Xna::Framework::Vector2 position,
                 Microsoft::Xna::Framework::Vector2 velocity,
                 Microsoft::Xna::Framework::Vector4 startColor,
                 Microsoft::Xna::Framework::Vector4 endColor,
                 System::TimeSpan lifetime);

        [[nodiscard]] const Microsoft::Xna::Framework::Vector4& getColorProperty() const;
        void Update(System::TimeSpan time, System::TimeSpan elapsedTime) override;

    private:
        Microsoft::Xna::Framework::Vector4 color_;
        Microsoft::Xna::Framework::Vector4 startColor_;
        Microsoft::Xna::Framework::Vector4 endColor_;
        System::TimeSpan endTime_ = System::TimeSpan::Zero;
        System::TimeSpan lifetime_;
    };
}
