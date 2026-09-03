// SPDX-License-Identifier: MS-PL

#include "Helpers/Log.hpp"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <memory>
#include <mutex>

#include "System/IO/FileMode.hpp"
#include "System/IO/IsolatedStorage/IsolatedStorageFileStream.hpp"
#include "System/IO/StreamWriter.hpp"
#include "System/DateTime.hpp"

namespace RacingGame::Helpers
{
    namespace
    {
        using System::IO::FileMode;
        using System::IO::StreamWriter;
        using System::IO::IsolatedStorage::IsolatedStorageFileStream;

        constexpr const char* LogFilename = "Log.txt";
        constexpr int MaximumLogBytes = 2 * 1024 * 1024;
        std::mutex logMutex;
        std::unique_ptr<IsolatedStorageFileStream> logStream;
        std::unique_ptr<StreamWriter> logWriter;

        std::tm LocalTime(const std::time_t value)
        {
            std::tm result{};
#ifdef _WIN32
            localtime_s(&result, &value);
#else
            localtime_r(&value, &result);
#endif
            return result;
        }

        void WriteUnlocked(const std::string& message)
        {
            if (!logWriter) return;
            const std::time_t now = std::chrono::system_clock::to_time_t(
                std::chrono::system_clock::now());
            const std::tm local = LocalTime(now);
            char timestamp[16]{};
            std::snprintf(timestamp, sizeof(timestamp), "[%02d:%02d:%02d] ",
                          local.tm_hour, local.tm_min, local.tm_sec);
            logWriter->WriteLine(std::string(timestamp) + message);
            logWriter->Flush();
#ifndef NDEBUG
            std::printf("%s%s\n", timestamp, message.c_str());
#endif
        }
    }

    void Log::Initialize()
    {
        std::scoped_lock lock(logMutex);
        try
        {
            logWriter.reset();
            logStream.reset();
            logStream = std::make_unique<IsolatedStorageFileStream>(
                LogFilename, FileMode::OpenOrCreate);
            if (logStream->getLengthProperty() > MaximumLogBytes)
            {
                logStream->Close();
                logStream = std::make_unique<IsolatedStorageFileStream>(
                    LogFilename, FileMode::Create);
            }
            logStream->setPositionProperty(logStream->getLengthProperty());
            logWriter = std::make_unique<StreamWriter>(logStream.get(), true);
            logWriter->WriteLine("");
            logWriter->WriteLine(
                "/// Session started at: " +
                System::DateTime::getNowProperty().ToString());
            logWriter->WriteLine("/// RacingGame");
            logWriter->WriteLine("");
            logWriter->Flush();
        }
        catch (...)
        {
            logWriter.reset();
            logStream.reset();
        }
    }

    void Log::Write(const std::string& message)
    {
        std::scoped_lock lock(logMutex);
        try
        {
            WriteUnlocked(message);
        }
        catch (...)
        {
        }
    }
}
