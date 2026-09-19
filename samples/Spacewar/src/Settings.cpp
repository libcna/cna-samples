// SPDX-License-Identifier: MS-PL

#include "Settings.hpp"

#include "System/IO/File.hpp"

namespace Spacewar
{
    void Settings::Save(const std::string& filename) const
    {
        auto stream = System::IO::File::Create(filename);
        System::Xml::Serialization::XmlSerializer<Settings>{}.Serialize(stream, *this);
    }

    Settings Settings::Load(const std::string& filename)
    {
        auto stream = System::IO::File::OpenRead(filename);
        return System::Xml::Serialization::XmlSerializer<Settings>{}.Deserialize(stream);
    }
}
