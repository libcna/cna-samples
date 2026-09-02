// SPDX-License-Identifier: MS-PL

#pragma once

#include <utility>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "Serialization/XmlSerializationAdapters.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Collections/Generic/List.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

namespace RacingGame::Tracks
{
    using SharpRuntime::String;

    class TrackData
    {
    public:
        static constexpr const char* Directory = "Content";
        static constexpr const char* Extension = "Track";

        class WidthHelper
        {
        public:
            Microsoft::Xna::Framework::Vector3 pos;
            float scale = 0.0f;

            WidthHelper() = default;
            WidthHelper(Microsoft::Xna::Framework::Vector3 setPos, float setScale)
                : pos(setPos), scale(setScale)
            {
            }

            SHARP_XML_SERIALIZABLE(
                WidthHelper, "WidthHelper",
                SHARP_XML_M(WidthHelper, pos), SHARP_XML_M(WidthHelper, scale))
        };

        class RoadHelper
        {
        public:
            enum class HelperType
            {
                Tunnel,
                Palms,
                Laterns,
                Reset,
            };

            HelperType type = HelperType::Tunnel;
            Microsoft::Xna::Framework::Vector3 pos;

            RoadHelper() = default;
            RoadHelper(HelperType setType, Microsoft::Xna::Framework::Vector3 setPos)
                : type(setType), pos(setPos)
            {
            }

            SHARP_XML_SERIALIZABLE(
                RoadHelper, "RoadHelper",
                SHARP_XML_M(RoadHelper, type), SHARP_XML_M(RoadHelper, pos))
        };

        class NeutralObject
        {
        public:
            String modelName;
            Microsoft::Xna::Framework::Matrix matrix;

            NeutralObject() = default;
            NeutralObject(String setModelName, Microsoft::Xna::Framework::Matrix setMatrix)
                : modelName(std::move(setModelName)), matrix(setMatrix)
            {
            }

            SHARP_XML_SERIALIZABLE(
                NeutralObject, "NeutralObject",
                SHARP_XML_M(NeutralObject, modelName),
                SHARP_XML_M(NeutralObject, matrix))
        };

        using TrackPointList = System::Collections::Generic::List<
            Microsoft::Xna::Framework::Vector3>;
        using WidthHelperList = System::Collections::Generic::List<WidthHelper>;
        using RoadHelperList = System::Collections::Generic::List<RoadHelper>;
        using NeutralObjectList = System::Collections::Generic::List<NeutralObject>;

        TrackData() = default;
        TrackData(TrackPointList setTrackPoints,
                  WidthHelperList setWidthHelpers,
                  RoadHelperList setRoadHelpers,
                  NeutralObjectList setObjects);

        [[nodiscard]] TrackPointList& getTrackPointsProperty();
        [[nodiscard]] const TrackPointList& getTrackPointsProperty() const;
        [[nodiscard]] WidthHelperList& getWidthHelpersProperty();
        [[nodiscard]] const WidthHelperList& getWidthHelpersProperty() const;
        [[nodiscard]] RoadHelperList& getRoadHelpersProperty();
        [[nodiscard]] const RoadHelperList& getRoadHelpersProperty() const;
        [[nodiscard]] NeutralObjectList& getNeutralsObjectsProperty();
        [[nodiscard]] const NeutralObjectList& getNeutralsObjectsProperty() const;

        [[nodiscard]] static TrackData Load(const String& setFilename);

    private:
        TrackPointList trackPoints;
        WidthHelperList widthHelpers;
        RoadHelperList roadHelpers;
        NeutralObjectList objects;

        SHARP_XML_SERIALIZABLE(
            TrackData, "TrackData",
            ::System::Xml::Serialization::detail::MakeMember(
                "TrackPoints", &TrackData::trackPoints),
            ::System::Xml::Serialization::detail::MakeMember(
                "WidthHelpers", &TrackData::widthHelpers),
            ::System::Xml::Serialization::detail::MakeMember(
                "RoadHelpers", &TrackData::roadHelpers),
            ::System::Xml::Serialization::detail::MakeMember(
                "NeutralsObjects", &TrackData::objects))
    };

    SHARP_XML_ENUM(
        TrackData::RoadHelper::HelperType,
        SHARP_XML_E(TrackData::RoadHelper::HelperType, Tunnel),
        SHARP_XML_E(TrackData::RoadHelper::HelperType, Palms),
        SHARP_XML_E(TrackData::RoadHelper::HelperType, Laterns),
        SHARP_XML_E(TrackData::RoadHelper::HelperType, Reset))
}
