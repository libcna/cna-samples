// SPDX-License-Identifier: MS-PL
#pragma once

// IDebugCommandHost.hpp — C++ port of GameDebugTools/IDebugCommandHost.cs
// (XNA 4.0 PerformanceMeasuring sample).

#include <functional>
#include <string>
#include <vector>

namespace PerformanceMeasuring::GameDebugTools {

class IDebugCommandHost;

/** @brief Severity of a message echoed to the debug command output. */
enum class DebugCommandMessage {
    /** @brief Ordinary output. */
    Standard = 1,
    /** @brief An error; drawn in the error colour. */
    Error = 2,
    /** @brief A warning; drawn in the warning colour. */
    Warning = 3
};

/**
 * @brief Callback invoked when a registered debug command is executed.
 *
 * Receives the host it was registered with, the command word itself, and the arguments that
 * followed it on the command line.
 */
using DebugCommandExecute =
    std::function<void(IDebugCommandHost& host, const std::string& command,
                        const std::vector<std::string>& arguments)>;

/** @brief Something that can execute a debug command line. */
class IDebugCommandExecutioner {
public:
    /** @brief Destroys the executioner. */
    virtual ~IDebugCommandExecutioner() = default;

    /**
     * @brief Executes one command line.
     *
     * @param command The whole line, command word and arguments together.
     */
    virtual void ExecuteCommand(const std::string& command) = 0;
};

/** @brief Something that wants to see every message the command host echoes. */
class IDebugEchoListner {
public:
    /** @brief Destroys the listener. */
    virtual ~IDebugEchoListner() = default;

    /**
     * @brief Receives one echoed message.
     *
     * @param messageType Severity of the message.
     * @param text        The message itself.
     */
    virtual void Echo(DebugCommandMessage messageType, const std::string& text) = 0;
};

/**
 * @brief The debug command host: owns the command table and the echo listeners.
 *
 * Registered as a game service, so any component can add its own commands to the console without
 * knowing which component draws it.
 */
class IDebugCommandHost : public IDebugEchoListner, public IDebugCommandExecutioner {
public:
    /**
     * @brief Adds a command to the table.
     *
     * @param command     Word the user types.
     * @param description One-line help shown by the `help` command.
     * @param callback    Invoked when the command is executed.
     */
    virtual void RegisterCommand(const std::string& command, const std::string& description,
                                  DebugCommandExecute callback) = 0;

    /**
     * @brief Removes a command from the table.
     *
     * @param command Word the command was registered under.
     */
    virtual void UnregisterCommand(const std::string& command) = 0;

    /** @brief Echoes an ordinary message. @param text The message. */
    virtual void Echo(const std::string& text) = 0;

    /** @brief Echoes a warning. @param text The message. */
    virtual void EchoWarning(const std::string& text) = 0;

    /** @brief Echoes an error. @param text The message. */
    virtual void EchoError(const std::string& text) = 0;

    /** @brief Starts sending echoed messages to a listener. @param listner The listener. */
    virtual void RegisterEchoListner(IDebugEchoListner* listner) = 0;

    /** @brief Stops sending echoed messages to a listener. @param listner The listener. */
    virtual void UnregisterEchoListner(IDebugEchoListner* listner) = 0;

    /**
     * @brief Redirects command execution to another executioner until it is popped.
     *
     * @param executioner Takes every command line while it is on top of the stack.
     */
    virtual void PushExecutioner(IDebugCommandExecutioner* executioner) = 0;

    /** @brief Removes the executioner most recently pushed. */
    virtual void PopExecutioner() = 0;
};

} // namespace PerformanceMeasuring::GameDebugTools
