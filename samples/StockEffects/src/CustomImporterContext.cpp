// SPDX-License-Identifier: MS-PL
#include "CustomImporterContext.hpp"

namespace CompileEffect
{
    CustomImporterContext::CustomImporterContext(
        Microsoft::Xna::Framework::Content::Pipeline::ContentBuildLogger& logger)
        : logger_(logger)
    {
    }

    std::string CustomImporterContext::getIntermediateDirectoryProperty() const { return {}; }

    Microsoft::Xna::Framework::Content::Pipeline::ContentBuildLogger&
        CustomImporterContext::getLoggerProperty() const
    {
        return logger_;
    }

    std::string CustomImporterContext::getOutputDirectoryProperty() const { return {}; }

    void CustomImporterContext::AddDependency(const std::string&) {}
}
