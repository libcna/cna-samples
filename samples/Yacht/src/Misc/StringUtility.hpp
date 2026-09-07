#pragma once

// StringUtility.hpp -- C++ port of Yacht/Misc/StringUtility.cs.

#include <regex>
#include <string>

namespace Yacht {

// Auxiliary methods for strings.
namespace StringUtility {

    // Check if a name is valid as a player or game name.
    //
    // The rule is the original's, character class and length limit included: letters, digits,
    // hyphen and apostrophe only, non-empty, and under ten characters. It is enforced on both
    // the player's own name and the name of a game they create.
    [[nodiscard]] inline bool IsNameValid(const std::string& name)
    {
        static const std::regex re("^[-'a-zA-Z0-9]*$");
        return !name.empty() && std::regex_match(name, re) && name.length() < 10;
    }

} // namespace StringUtility

} // namespace Yacht
