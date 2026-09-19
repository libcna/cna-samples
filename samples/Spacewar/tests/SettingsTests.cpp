// SPDX-License-Identifier: MS-PL

#include "Settings.hpp"

#include "System/IO/File.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: SpacewarSettingsTests <original-settings.xml> <output.xml>\n";
        return 2;
    }

    const Spacewar::Settings settings = Spacewar::Settings::Load(argv[1]);
    using Microsoft::Xna::Framework::Input::Keys;
    if (settings.MediaPath != "content\\" || settings.Ships.size() != 2 ||
        settings.Weapons.size() != 5 || settings.ShipLights.size() != 2 ||
        settings.Weapons[1].Max != 16 || settings.Weapons[3].Acceleration != 300.0f ||
        settings.ShipLights[0].Ambient.X != 0.4f ||
        settings.ShipLights[0].PointFactor != 0.0001f ||
        settings.Player1Start != Keys::LeftControl ||
        settings.Player1B != Keys::G || settings.Player2RightTrigger != Keys::Delete)
    {
        std::cerr << "The authentic Spacewar settings did not deserialize faithfully.\n";
        return 1;
    }

    settings.Save(argv[2]);
    const Spacewar::Settings roundTrip = Spacewar::Settings::Load(argv[2]);
    const System::Xml::Serialization::XmlSerializer<Spacewar::Settings> serializer;
    if (serializer.Serialize(settings) != serializer.Serialize(roundTrip))
    {
        std::cerr << "The saved settings did not round-trip.\n";
        return 1;
    }

    const std::string saved = System::IO::File::ReadAllText(argv[2]);
    if (saved.find("<?xml version=\"1.0\" encoding=\"utf-8\"?>") != 0 ||
        saved.find("<MediaPath>content\\</MediaPath>") == std::string::npos ||
        saved.find("<Player2RightTrigger>Delete</Player2RightTrigger>") == std::string::npos)
    {
        std::cerr << "The saved settings have an unexpected XML wire shape.\n";
        return 1;
    }

    std::cout << "Spacewar settings XML load/save round-trip passed.\n";
    return 0;
}
