// SPDX-License-Identifier: MS-PL
#pragma once

#include <memory>

#include "System/Guid.hpp"
#include "System/Random.hpp"

namespace TiltPerspectiveSample {

class RandomUtil {
public:
    [[nodiscard]] static System::Random NewRandom() {
        return System::Random(System::Guid::NewGuid().GetHashCode());
    }

    [[nodiscard]] static System::Random& getSharedRandomProperty() {
        if (!threadStaticRandom_)
            threadStaticRandom_ = std::make_unique<System::Random>(
                System::Guid::NewGuid().GetHashCode());
        return *threadStaticRandom_;
    }

    [[nodiscard]] static float NextFloat(System::Random& rng) {
        constexpr float scale = 1.0f / 2147483648.0f;
        return static_cast<float>(rng.Next() & 0x7fffffff) * scale;
    }

    [[nodiscard]] static float NextFloat(System::Random& rng, float max) {
        return NextFloat(rng) * max;
    }

    [[nodiscard]] static float NextFloat(System::Random& rng, float min, float max) {
        return NextFloat(rng) * (max - min) + min;
    }

private:
    inline static thread_local std::unique_ptr<System::Random> threadStaticRandom_;
};

} // namespace TiltPerspectiveSample
