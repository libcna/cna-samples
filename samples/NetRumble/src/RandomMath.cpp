// SPDX-License-Identifier: MS-PL
#include "RandomMath.hpp"

#include <cmath>

#include "Microsoft/Xna/Framework/MathHelper.hpp"

namespace NetRumble {
using namespace Microsoft::Xna::Framework;
System::Random RandomMath::random_;
System::Random &RandomMath::getRandomProperty() { return random_; }
float RandomMath::RandomBetween(float minimum, float maximum) {
  return minimum +
         static_cast<float>(random_.NextDouble()) * (maximum - minimum);
}
Vector2 RandomMath::RandomDirection() {
  const float angle = RandomBetween(0.0f, MathHelper::TwoPi);
  return Vector2(static_cast<float>(std::cos(angle)),
                 static_cast<float>(std::sin(angle)));
}
Vector2 RandomMath::RandomDirection(float minimumAngle, float maximumAngle) {
  const float angle = RandomBetween(MathHelper::ToRadians(minimumAngle),
                                    MathHelper::ToRadians(maximumAngle)) -
                      MathHelper::PiOver2;
  return Vector2(static_cast<float>(std::cos(angle)),
                 static_cast<float>(std::sin(angle)));
}
} // namespace NetRumble
