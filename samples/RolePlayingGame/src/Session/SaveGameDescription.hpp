#pragma once

// SaveGameDescription.hpp -- C++ port of RolePlayingGame/Session/SaveGameDescription.cs.

#include <string>

#include "System/Xml/Serialization/XmlSerializer.hpp"

namespace RolePlaying {

// The description of a save game file, written beside it so the save/load screen can list the
// saves without opening them.
class SaveGameDescription {
public:
    std::string FileName;
    std::string ChapterName;
    std::string Description;

    SHARP_XML_SERIALIZABLE(SaveGameDescription, "SaveGameDescription",
                           SHARP_XML_M(SaveGameDescription, FileName),
                           SHARP_XML_M(SaveGameDescription, ChapterName),
                           SHARP_XML_M(SaveGameDescription, Description))
};

} // namespace RolePlaying
