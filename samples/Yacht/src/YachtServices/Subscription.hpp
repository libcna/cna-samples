#pragma once

// Subscription.hpp -- C++ port of YachtServices/Subscription.cs.

#include <string>
#include <utility>

#include "System/Guid.hpp"
#include "System/Uri.hpp"

namespace YachtServices {

/**
 * @brief One registered client, as the server sees it.
 *
 * The URI is the whole point: it is where the server pushes to when this player's game moves,
 * and it is the client's own address rather than anything the server chose.
 */
class Subscription {
public:
    /**
     * @brief Creates a subscription.
     *
     * @param sessionID  The session the server issued.
     * @param channelUri Where to push notifications for this client.
     * @param name       The player's name.
     */
    Subscription(int sessionID, System::Uri channelUri, std::string name)
        : ChannelUri(std::move(channelUri)), Name(std::move(name)), sessionID_(sessionID)
    {
    }

    /** @brief Where to push notifications for this client. */
    System::Uri ChannelUri;

    /** @brief The game this client is in. */
    System::Guid GameID;

    /** @brief The player's name. */
    std::string Name;

    /**
     * @brief The session the server issued.
     *
     * @return The session ID.
     */
    [[nodiscard]] int getSessionIDProperty() const { return sessionID_; }

private:
    int sessionID_ = 0;
};

} // namespace YachtServices
