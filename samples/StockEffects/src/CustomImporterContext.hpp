// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Content/Pipeline/ContentImporterContext.hpp"

namespace CompileEffect
{
    /** @brief Minimal context used to invoke an XNA-shaped content importer directly. */
    class CustomImporterContext final
        : public Microsoft::Xna::Framework::Content::Pipeline::ContentImporterContext
    {
    public:
        /**
         * @brief Constructs the context.
         * @param logger Logger used by the importer.
         */
        explicit CustomImporterContext(
            Microsoft::Xna::Framework::Content::Pipeline::ContentBuildLogger& logger);

        /** @brief Gets the intermediate directory. */
        [[nodiscard]] std::string getIntermediateDirectoryProperty() const override;

        /** @brief Gets the importer logger. */
        [[nodiscard]] Microsoft::Xna::Framework::Content::Pipeline::ContentBuildLogger&
            getLoggerProperty() const override;

        /** @brief Gets the output directory. */
        [[nodiscard]] std::string getOutputDirectoryProperty() const override;

        /** @brief Records a dependency; this standalone tool keeps no dependency graph. */
        void AddDependency(const std::string& filename) override;

    private:
        Microsoft::Xna::Framework::Content::Pipeline::ContentBuildLogger& logger_;
    };
}
