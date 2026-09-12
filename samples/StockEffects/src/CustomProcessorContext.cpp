// SPDX-License-Identifier: MS-PL
#include "CustomProcessorContext.hpp"

#include "System/NotImplementedException.hpp"

namespace CompileEffect
{
    namespace Xna = Microsoft::Xna::Framework::Content::Pipeline;

    CustomProcessorContext::CustomProcessorContext(
        Xna::TargetPlatform targetPlatform,
        Microsoft::Xna::Framework::Graphics::GraphicsProfile targetProfile,
        Xna::ContentBuildLogger& logger)
        : targetPlatform_(targetPlatform)
        , targetProfile_(targetProfile)
        , logger_(logger)
    {
    }

    std::string CustomProcessorContext::getBuildConfigurationProperty() const { return {}; }
    std::string CustomProcessorContext::getIntermediateDirectoryProperty() const { return {}; }
    Xna::ContentBuildLogger& CustomProcessorContext::getLoggerProperty() const { return logger_; }
    std::string CustomProcessorContext::getOutputDirectoryProperty() const { return {}; }
    std::string CustomProcessorContext::getOutputFilenameProperty() const { return {}; }
    const Xna::OpaqueDataDictionary& CustomProcessorContext::getParametersProperty() const
    {
        return parameters_;
    }
    Xna::TargetPlatform CustomProcessorContext::getTargetPlatformProperty() const
    {
        return targetPlatform_;
    }
    Microsoft::Xna::Framework::Graphics::GraphicsProfile
        CustomProcessorContext::getTargetProfileProperty() const
    {
        return targetProfile_;
    }

    void CustomProcessorContext::AddDependency(const std::string&) {}
    void CustomProcessorContext::AddOutputFile(const std::string&) {}

    Xna::ContentObject CustomProcessorContext::BuildAndLoadAssetCore(
        const std::string&, const Xna::ContentIdentity&, const std::string&,
        const Xna::OpaqueDataDictionary&, const std::string&, const std::string&,
        const std::string&)
    {
        throw System::NotImplementedException();
    }

    std::string CustomProcessorContext::BuildAssetCore(
        const std::string&, const Xna::ContentIdentity&, const std::string&,
        const Xna::OpaqueDataDictionary&, const std::string&, const std::string&,
        const std::string&, const std::string&)
    {
        throw System::NotImplementedException();
    }

    Xna::ContentObject CustomProcessorContext::ConvertCore(
        const Xna::ContentObject&, const std::string&, const Xna::OpaqueDataDictionary&,
        const std::string&, const std::string&)
    {
        throw System::NotImplementedException();
    }
}
