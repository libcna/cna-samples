// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"

#include <optional>

#include "Microsoft/Xna/Framework/Net/AvailableNetworkSessionCollection.hpp"
#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "OperationCompletedEventArgs.hpp"
#include "ScreenManager/MenuScreen.hpp"

namespace NetRumble {
/** @brief Lists the sessions returned by an asynchronous network search. */
class SearchResultsScreen final : public MenuScreen {
public:
  /** @brief Constructs a result screen. @param sessionType Search type. */
  explicit SearchResultsScreen(
      Microsoft::Xna::Framework::Net::NetworkSessionType sessionType);

  /** @brief Validates sign-in state and updates transitions. @param gameTime
   * Timing. @param otherScreenHasFocus Focus state. @param coveredByOtherScreen
   * Coverage state. */
  void Update(Microsoft::Xna::Framework::GameTime &gameTime,
              bool otherScreenHasFocus, bool coveredByOtherScreen) override;

  /** @brief Draws results or a search status message. @param gameTime Timing.
   */
  void Draw(const Microsoft::Xna::Framework::GameTime &gameTime) override;

  /** @brief Receives the completed search operation. @param sender Event
   * sender. @param e Completed operation. */
  void SessionsFound(System::Object *sender,
                     const OperationCompletedEventArgs &e);

  /** @brief Returns the managed type name. @return Type name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override;

protected:
  /** @brief Joins the selected session. @param entryIndex Selected result. */
  void OnSelectEntry(int entryIndex) override;

  /** @brief Leaves the result screen. */
  void OnCancel() override;

private:
  static constexpr int maximumSessions_ = 8;
  void LoadLobbyScreen(System::Object *sender,
                       const OperationCompletedEventArgs &e);
  void FailedMessageBox(System::Object *sender, const System::EventArgs &e);
  void ShowFailure(const std::string &message);

  Microsoft::Xna::Framework::Net::NetworkSessionType sessionType_;
  std::optional<
      Microsoft::Xna::Framework::Net::AvailableNetworkSessionCollection>
      availableSessions_;
};
} // namespace NetRumble
