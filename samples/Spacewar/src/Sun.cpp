// SPDX-License-Identifier: MS-PL

#include "Sun.hpp"

#include "EvolvedSun.hpp"
#include "Shape.hpp"

namespace Spacewar
{
    using namespace Microsoft::Xna::Framework;

    Sun::Sun(Game* game, std::unique_ptr<Shape> shape, Vector3 position)
        : SceneItem(game, std::move(shape), position)
    {
        if (!shape_ || dynamic_cast<EvolvedSun*>(shape_.get()))
        {
            center_ = Vector3(0.5f, 0.5f, 0.0f);
            rotation_ = Vector3::Zero;
            radius_ = 15.0f;
        }
        else
        {
            scale_ = Vector3(8.0f, 8.0f, 8.0f);
            radius_ = 11.0f;
        }
    }

    void Sun::Update(System::TimeSpan time, System::TimeSpan elapsedTime)
    {
        rotation_.Z += static_cast<float>(elapsedTime.getTotalSecondsProperty() / 10.0);
        SceneItem::Update(time, elapsedTime);
    }
}
