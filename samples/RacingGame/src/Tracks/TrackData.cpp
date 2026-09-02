// SPDX-License-Identifier: MS-PL

#include "Tracks/TrackData.hpp"

#include "Microsoft/Xna/Framework/TitleContainer.hpp"
#include "System/IO/StreamReader.hpp"

namespace RacingGame::Tracks
{
    using Microsoft::Xna::Framework::TitleContainer;
    using System::IO::StreamReader;
    using System::Xml::Serialization::XmlSerializer;

    TrackData::TrackData(TrackPointList setTrackPoints,
                         WidthHelperList setWidthHelpers,
                         RoadHelperList setRoadHelpers,
                         NeutralObjectList setObjects)
        : trackPoints(std::move(setTrackPoints)),
          widthHelpers(std::move(setWidthHelpers)),
          roadHelpers(std::move(setRoadHelpers)),
          objects(std::move(setObjects))
    {
    }

    TrackData::TrackPointList& TrackData::getTrackPointsProperty()
    {
        return trackPoints;
    }

    const TrackData::TrackPointList& TrackData::getTrackPointsProperty() const
    {
        return trackPoints;
    }

    TrackData::WidthHelperList& TrackData::getWidthHelpersProperty()
    {
        return widthHelpers;
    }

    const TrackData::WidthHelperList& TrackData::getWidthHelpersProperty() const
    {
        return widthHelpers;
    }

    TrackData::RoadHelperList& TrackData::getRoadHelpersProperty()
    {
        return roadHelpers;
    }

    const TrackData::RoadHelperList& TrackData::getRoadHelpersProperty() const
    {
        return roadHelpers;
    }

    TrackData::NeutralObjectList& TrackData::getNeutralsObjectsProperty()
    {
        return objects;
    }

    const TrackData::NeutralObjectList& TrackData::getNeutralsObjectsProperty() const
    {
        return objects;
    }

    TrackData TrackData::Load(const String& setFilename)
    {
        auto file = TitleContainer::OpenStream(
            String(Directory) + "\\" + setFilename + "." + Extension);
        StreamReader reader(file.get(), true);
        const String xml = reader.ReadToEnd();
        reader.Close();
        file->Close();
        return XmlSerializer<TrackData>{}.Deserialize(xml);
    }
}
