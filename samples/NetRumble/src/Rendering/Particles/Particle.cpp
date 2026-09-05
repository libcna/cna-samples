// SPDX-License-Identifier: MS-PL
#include "Rendering/Particles/Particle.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
namespace NetRumble {
void Particle::Update(float e, float av, float sd, float od) {
  Velocity.X += Acceleration.X * e;
  Velocity.Y += Acceleration.Y * e;
  Position.X += Velocity.X * e;
  Position.Y += Velocity.Y * e;
  Rotation += av * e;
  Scale += sd * e;
  if (Scale < 0)
    Scale = 0;
  Opacity =
      Microsoft::Xna::Framework::MathHelper::Clamp(Opacity + od * e, 0.f, 1.f);
}
} // namespace NetRumble
