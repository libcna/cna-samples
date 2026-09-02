// SPDX-License-Identifier: MS-PL

#pragma once

#include <cstdint>
#include <vector>

#include "Graphics/TangentVertex.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"

namespace RacingGame::Tracks
{
    class TrackLine;

    /** @brief Owns the CPU mesh and holder transforms for one track guard rail. */
    class GuardRailGeometry
    {
    public:
        /** @brief Selects the left or right edge of the road. */
        enum class Mode
        {
            /** @brief Generates the left guard rail. */
            Left,
            /** @brief Generates the right guard rail. */
            Right,
        };

        /**
         * @brief Generates a guard rail from a completed track line.
         *
         * @param trackLine Source road line.
         * @param mode Edge on which the rail is generated.
         */
        GuardRailGeometry(const TrackLine& trackLine, Mode mode);

        /** @brief Gets the number of sampled rail points. */
        [[nodiscard]] std::size_t getRailPointCountProperty() const;
        /** @brief Gets the generated guard-rail vertices. */
        [[nodiscard]] const std::vector<Graphics::TangentVertex>&
        getVerticesProperty() const;
        /** @brief Gets the generated guard-rail triangle indices. */
        [[nodiscard]] const std::vector<std::int32_t>& getIndicesProperty() const;
        /** @brief Gets transforms for the original guard-rail holder model. */
        [[nodiscard]] const std::vector<Microsoft::Xna::Framework::Matrix>&
        getHolderMatricesProperty() const;

    private:
        std::size_t railPointCount = 0;
        std::vector<Graphics::TangentVertex> vertices;
        std::vector<std::int32_t> indices;
        std::vector<Microsoft::Xna::Framework::Matrix> holderMatrices;
    };
}
