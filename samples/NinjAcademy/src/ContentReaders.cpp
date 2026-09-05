// SPDX-License-Identifier: MS-PL
// CNAEXT -- C++ supplies the reflection metadata that XNA obtains from managed types.

#include "ContentReaders.hpp"

#include <memory>
#include <string>

#include "Animation.hpp"
#include "AnimationStore.hpp"
#include "GameConfiguration.hpp"
#include "CNA/Internal/Xnb/CollectionContentTypeReaders.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReaderManager.hpp"
#include "Microsoft/Xna/Framework/Content/ReflectiveTypeReader.hpp"

namespace NinjAcademy {

using CNA::Internal::Xnb::ArrayReader;
using CNA::Internal::Xnb::DictionaryReader;
using CNA::Internal::Xnb::ListReader;
using Microsoft::Xna::Framework::Content::ContentTypeReaderManager;
using Microsoft::Xna::Framework::Content::ReflectiveTypeReader;
using Microsoft::Xna::Framework::Content::ReflectiveTypeReaderBuilder;

namespace {
const std::string AnimationTypeName = "NinjAcademy.Animation";
const std::string AnimationStoreTypeName = "NinjAcademy.AnimationStore";
const std::string GamePhaseTypeName = "NinjAcademy.GamePhase";
const std::string GameConfigurationTypeName = "NinjAcademy.GameConfiguration";
}

void NinjAcademyContentReaderRegistrationEXT::RegisterEXT() {
    ReflectiveTypeReaderBuilder<Animation>(AnimationTypeName)
        .Field(&Animation::frameCount_)
        .Field(&Animation::visualCenter_)
        .Field(&Animation::isActive_)
        .Field(&Animation::animationSheetPath_)
        .Field(&Animation::rowAndColumnAmount_)
        .Field(&Animation::frameSize_)
        .Field(&Animation::isCyclic_)
        .RegisterShared();

    ContentTypeReaderManager::AddTypeCreator(
        "Microsoft.Xna.Framework.Content.DictionaryReader`2[[System.String],[NinjAcademy.Animation]]",
        [] {
            return std::make_unique<DictionaryReader<std::string, std::shared_ptr<Animation>>>(
                "System.Collections.Generic.Dictionary`2[[System.String],[NinjAcademy.Animation]]",
                "Microsoft.Xna.Framework.Content.StringReader",
                ReflectiveTypeReader<Animation>::CanonicalReaderName(AnimationTypeName));
        });

    ReflectiveTypeReaderBuilder<AnimationStore>(AnimationStoreTypeName)
        .Field(&AnimationStore::animations_)
        .Register();

    ContentTypeReaderManager::AddTypeCreator(
        "Microsoft.Xna.Framework.Content.ArrayReader`1[[System.TimeSpan]]",
        [] {
            return std::make_unique<ArrayReader<System::TimeSpan>>(
                "System.TimeSpan[]", "Microsoft.Xna.Framework.Content.TimeSpanReader");
        });

    ContentTypeReaderManager::AddTypeCreator(
        "Microsoft.Xna.Framework.Content.ArrayReader`1[[System.Double]]",
        [] {
            return std::make_unique<ArrayReader<double>>(
                "System.Double[]", "Microsoft.Xna.Framework.Content.DoubleReader");
        });

    ReflectiveTypeReaderBuilder<GamePhase>(GamePhaseTypeName)
        .Field(&GamePhase::duration_)
        .Field(&GamePhase::targetAppearanceIntervals_)
        .Field(&GamePhase::targetAppearanceProbabilities_)
        .Field(&GamePhase::goldTargetProbablity_)
        .Field(&GamePhase::bambooAppearanceInterval_)
        .Field(&GamePhase::bambooAppearanceProbablity_)
        .Field(&GamePhase::dynamiteAppearanceInterval_)
        .Field(&GamePhase::dynamiteAppearanceProbablity_)
        .Field(&GamePhase::dynamiteAmountProbabilities_)
        .RegisterShared();

    ContentTypeReaderManager::AddTypeCreator(
        "Microsoft.Xna.Framework.Content.ListReader`1[[NinjAcademy.GamePhase]]",
        [] {
            return std::make_unique<ListReader<std::shared_ptr<GamePhase>>>(
                "System.Collections.Generic.List`1[[NinjAcademy.GamePhase]]",
                ReflectiveTypeReader<GamePhase>::CanonicalReaderName(GamePhaseTypeName));
        });

    ReflectiveTypeReaderBuilder<GameConfiguration>(GameConfigurationTypeName)
        .Field(&GameConfiguration::playerLives_)
        .Field(&GameConfiguration::pointsPerTarget_)
        .Field(&GameConfiguration::pointsPerGoldTarget_)
        .Field(&GameConfiguration::pointsPerBamboo_)
        .Field(&GameConfiguration::phases_)
        .Register();
}

} // namespace NinjAcademy
