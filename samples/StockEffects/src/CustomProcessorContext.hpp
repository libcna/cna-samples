// SPDX-License-Identifier: MS-PL
#pragma once

#include "Microsoft/Xna/Framework/Content/Pipeline/ContentProcessorContext.hpp"

namespace CompileEffect
{
    /** @brief Minimal context used to invoke the XNA-shaped EffectProcessor directly. */
    class CustomProcessorContext final
        : public Microsoft::Xna::Framework::Content::Pipeline::ContentProcessorContext
    {
    public:
        /**
         * @brief Constructs the processor context.
         * @param targetPlatform Requested XNA target platform.
         * @param targetProfile Requested graphics profile.
         * @param logger Logger used by the processor.
         */
        CustomProcessorContext(
            Microsoft::Xna::Framework::Content::Pipeline::TargetPlatform targetPlatform,
            Microsoft::Xna::Framework::Graphics::GraphicsProfile targetProfile,
            Microsoft::Xna::Framework::Content::Pipeline::ContentBuildLogger& logger);

        /** @brief Gets the build configuration. */
        [[nodiscard]] std::string getBuildConfigurationProperty() const override;
        /** @brief Gets the intermediate directory. */
        [[nodiscard]] std::string getIntermediateDirectoryProperty() const override;
        /** @brief Gets the processor logger. */
        [[nodiscard]] Microsoft::Xna::Framework::Content::Pipeline::ContentBuildLogger&
            getLoggerProperty() const override;
        /** @brief Gets the output directory. */
        [[nodiscard]] std::string getOutputDirectoryProperty() const override;
        /** @brief Gets the output filename. */
        [[nodiscard]] std::string getOutputFilenameProperty() const override;
        /** @brief Gets the processor parameter dictionary. */
        [[nodiscard]] const Microsoft::Xna::Framework::Content::Pipeline::OpaqueDataDictionary&
            getParametersProperty() const override;
        /** @brief Gets the requested XNA target platform. */
        [[nodiscard]] Microsoft::Xna::Framework::Content::Pipeline::TargetPlatform
            getTargetPlatformProperty() const override;
        /** @brief Gets the requested graphics profile. */
        [[nodiscard]] Microsoft::Xna::Framework::Graphics::GraphicsProfile
            getTargetProfileProperty() const override;

        /** @brief Records a dependency; this standalone tool keeps no dependency graph. */
        void AddDependency(const std::string& filename) override;
        /** @brief Records an output; this standalone tool writes its one output itself. */
        void AddOutputFile(const std::string& filename) override;

    protected:
        /** @brief Nested build-and-load is unavailable in this minimal context. */
        [[nodiscard]] Microsoft::Xna::Framework::Content::Pipeline::ContentObject
            BuildAndLoadAssetCore(
                const std::string& sourceFilename,
                const Microsoft::Xna::Framework::Content::Pipeline::ContentIdentity& sourceIdentity,
                const std::string& processorName,
                const Microsoft::Xna::Framework::Content::Pipeline::OpaqueDataDictionary& processorParameters,
                const std::string& importerName,
                const std::string& inputTypeName,
                const std::string& outputTypeName) override;

        /** @brief Nested asset builds are unavailable in this minimal context. */
        [[nodiscard]] std::string BuildAssetCore(
            const std::string& sourceFilename,
            const Microsoft::Xna::Framework::Content::Pipeline::ContentIdentity& sourceIdentity,
            const std::string& processorName,
            const Microsoft::Xna::Framework::Content::Pipeline::OpaqueDataDictionary& processorParameters,
            const std::string& importerName,
            const std::string& assetName,
            const std::string& inputTypeName,
            const std::string& outputTypeName) override;

        /** @brief In-memory conversion is unavailable in this minimal context. */
        [[nodiscard]] Microsoft::Xna::Framework::Content::Pipeline::ContentObject ConvertCore(
            const Microsoft::Xna::Framework::Content::Pipeline::ContentObject& input,
            const std::string& processorName,
            const Microsoft::Xna::Framework::Content::Pipeline::OpaqueDataDictionary& processorParameters,
            const std::string& inputTypeName,
            const std::string& outputTypeName) override;

    private:
        Microsoft::Xna::Framework::Content::Pipeline::TargetPlatform targetPlatform_;
        Microsoft::Xna::Framework::Graphics::GraphicsProfile targetProfile_;
        Microsoft::Xna::Framework::Content::Pipeline::ContentBuildLogger& logger_;
        Microsoft::Xna::Framework::Content::Pipeline::OpaqueDataDictionary parameters_;
    };
}
