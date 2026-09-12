// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Content/Pipeline/ContentBuildLogger.hpp"

namespace CompileEffect
{
    /** @brief Prints pipeline messages and promotes every warning to an exception. */
    class CustomLogger final
        : public Microsoft::Xna::Framework::Content::Pipeline::ContentBuildLogger
    {
    public:
        using ContentBuildLogger::LogImportantMessage;
        using ContentBuildLogger::LogMessage;
        using ContentBuildLogger::LogWarning;

        /** @brief Prints a high-priority content-build message. */
        void LogImportantMessage(const std::string& message) override;

        /** @brief Prints a low-priority content-build message. */
        void LogMessage(const std::string& message) override;

        /**
         * @brief Converts a content-build warning into an exception.
         * @param helpLink Warning help link.
         * @param contentIdentity Identity of the content that raised the warning.
         * @param message Warning text.
         */
        void LogWarning(
            const std::string& helpLink,
            const Microsoft::Xna::Framework::Content::Pipeline::ContentIdentity& contentIdentity,
            const std::string& message) override;
    };
}
