// SPDX-License-Identifier: MS-PL
#include "CustomLogger.hpp"

#include <iostream>

#include "System/Exception.hpp"

namespace CompileEffect
{
    void CustomLogger::LogImportantMessage(const std::string& message)
    {
        std::cout << message << '\n';
    }

    void CustomLogger::LogMessage(const std::string& message)
    {
        std::cout << message << '\n';
    }

    void CustomLogger::LogWarning(
        const std::string&,
        const Microsoft::Xna::Framework::Content::Pipeline::ContentIdentity&,
        const std::string& message)
    {
        throw System::Exception("Warning: " + message);
    }
}
