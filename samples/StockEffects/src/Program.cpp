// SPDX-License-Identifier: MS-PL

#include <algorithm>
#include <cctype>
#include <exception>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

#include "CustomImporterContext.hpp"
#include "CustomLogger.hpp"
#include "CustomProcessorContext.hpp"
#include "Microsoft/Xna/Framework/Content/Pipeline/EffectImporter.hpp"
#include "Microsoft/Xna/Framework/Content/Pipeline/Processors/EffectProcessor.hpp"
#include "Microsoft/Xna/Framework/Content/Pipeline/TargetPlatform.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsProfile.hpp"
#include "System/IO/File.hpp"

namespace CompileEffect
{
    namespace Xna = Microsoft::Xna::Framework::Content::Pipeline;
    namespace Graphics = Microsoft::Xna::Framework::Graphics;

    [[nodiscard]] std::optional<Graphics::GraphicsProfile> TryParseGraphicsProfile(std::string name)
    {
        std::ranges::transform(name, name.begin(), [](const unsigned char value)
        {
            return static_cast<char>(std::tolower(value));
        });
        if (name == "reach") { return Graphics::GraphicsProfile::Reach; }
        if (name == "hidef") { return Graphics::GraphicsProfile::HiDef; }
        return std::nullopt;
    }

    [[nodiscard]] const char* GraphicsProfileName(const Graphics::GraphicsProfile profile)
    {
        return profile == Graphics::GraphicsProfile::HiDef ? "HiDef" : "Reach";
    }

    int Run(const int argc, char** argv)
    {
        if (argc != 5)
        {
            std::cerr << "Usage: CompileEffect <targetPlatform> <targetProfile> <input.fx> <output.bin>\n";
            return 1;
        }

        const std::optional<Xna::TargetPlatform> targetPlatform =
            Xna::TryParseTargetPlatform(argv[1]);
        if (!targetPlatform.has_value())
        {
            std::cerr << "Invalid target platform " << argv[1]
                      << ". Valid options are Windows, Xbox360, WindowsPhone.\n";
            return 1;
        }

        const std::optional<Graphics::GraphicsProfile> targetProfile =
            TryParseGraphicsProfile(argv[2]);
        if (!targetProfile.has_value())
        {
            std::cerr << "Invalid target profile " << argv[2]
                      << ". Valid options are Reach, HiDef.\n";
            return 1;
        }

        const std::string inputFilename = argv[3];
        const std::string outputFilename = argv[4];

        try
        {
            std::cout << "Compiling " << std::filesystem::path(inputFilename).filename().string()
                      << " -> " << outputFilename << " for "
                      << Xna::TargetPlatformName(*targetPlatform) << ", "
                      << GraphicsProfileName(*targetProfile) << '\n';

            CustomLogger logger;
            Xna::EffectImporter importer;
            CustomImporterContext importerContext(logger);
            const auto sourceEffect = importer.Import(inputFilename, importerContext);

            Xna::Processors::EffectProcessor processor;
            CustomProcessorContext processorContext(*targetPlatform, *targetProfile, logger);
            const auto compiledEffect = processor.Process(sourceEffect, processorContext);

            System::IO::File::WriteAllBytes(outputFilename, compiledEffect->GetEffectCode());
        }
        catch (const std::exception& error)
        {
            std::cerr << "Error: " << error.what() << '\n';
            return 1;
        }

        return 0;
    }
}

int main(const int argc, char** argv)
{
    return CompileEffect::Run(argc, argv);
}
