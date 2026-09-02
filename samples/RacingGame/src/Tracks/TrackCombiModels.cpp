// SPDX-License-Identifier: MS-PL

#include "Tracks/TrackCombiModels.hpp"

#include "Landscapes/Landscape.hpp"
#include "Microsoft/Xna/Framework/TitleContainer.hpp"
#include "System/IO/Path.hpp"
#include "System/IO/StreamReader.hpp"

namespace RacingGame::Tracks
{
    using Landscapes::Landscape;
    using Microsoft::Xna::Framework::Matrix;
    using Microsoft::Xna::Framework::TitleContainer;
    using System::IO::Path;
    using System::IO::StreamReader;
    using System::Xml::Serialization::XmlSerializer;

    TrackCombiModels::TrackCombiModels(const String& filename)
    {
        auto file = TitleContainer::OpenStream(
            String(Directory) + "\\" + filename + "." + Extension);
        StreamReader reader(file.get(), true);
        const String xml = reader.ReadToEnd();
        objects = XmlSerializer<CombiObjectList>{}.Deserialize(xml);
        reader.Close();
        file->Close();

        name = Path::GetFileNameWithoutExtension(filename);
        size = name == "CombiPalms" || name == "CombiPalms2" ||
                       name == "CombiRuins" || name == "CombiRuins2" ||
                       name == "CombiStones" || name == "CombiStones2"
                   ? 10.0f
                   : 50.0f;
    }

    const String& TrackCombiModels::getNameProperty() const
    {
        return name;
    }

    float TrackCombiModels::getSizeProperty() const
    {
        return size;
    }

    void TrackCombiModels::AddAllModels(
        Landscape& landscape, Matrix parentMatrix) const
    {
        for (const CombiObject& object : objects)
        {
            landscape.AddObjectToRender(
                object.modelName, object.matrix * parentMatrix, false);
        }
    }
}
