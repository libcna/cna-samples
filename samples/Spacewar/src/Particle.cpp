// SPDX-License-Identifier: MS-PL

#include "Particle.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;

    Particle::Particle(Game* game, Vector2 position, Vector2 velocity, Vector4 startColor,
                       Vector4 endColor, System::TimeSpan lifetime)
        : SceneItem(game, Vector3(position, 0.0f)), startColor_(startColor), endColor_(endColor), lifetime_(lifetime)
    {
        velocity_ = Vector3(velocity, 0.0f);
    }

    const Vector4& Particle::getColorProperty() const { return color_; }

    void Particle::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        if (endTime_ == System::TimeSpan::Zero)
            endTime_ = time + lifetime_;
        if (time > endTime_)
            delete_ = true;
        const float percentLife = static_cast<float>(
            (endTime_.getTotalSecondsProperty() - time.getTotalSecondsProperty()) /
            lifetime_.getTotalSecondsProperty());
        color_ = Vector4::Lerp(endColor_, startColor_, percentLife);
        SceneItem::Update(time, elapsedTime);
    }
}
