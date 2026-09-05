// SPDX-License-Identifier: MS-PL
#pragma once

#include "CNA/CNAHelper.hpp"

#include "Microsoft/Xna/Framework/Net/NetworkSessionType.hpp"
#include "OperationCompletedEventArgs.hpp"
#include "ScreenManager/MenuScreen.hpp"

namespace Microsoft::Xna::Framework::Net {
class NetworkSession;
}

namespace NetRumble {
/** @brief Net Rumble's sign-in-aware top-level menu. */
class MainMenuScreen final : public MenuScreen {
public:
  /** @brief Constructs the main menu. */
  MainMenuScreen();

  /** @brief Refreshes menu state from gamer sign-in state. @param gameTime
   * Timing. @param otherScreenHasFocus Focus state. @param coveredByOtherScreen
   * Coverage state. */
  void Update(Microsoft::Xna::Framework::GameTime &gameTime,
              bool otherScreenHasFocus, bool coveredByOtherScreen) override;

  /** @brief Begins joining an accepted invitation. */
  void JoinInvitedGame();

  /** @brief Updates signed-in gamer presence. */
  void UpdatePresence() override;

  /** @brief Returns the managed type name. @return Type name. */
  CNAEXT [[nodiscard]] const std::string &GetTypeName() const override;

protected:
  /** @brief Handles one selected entry. @param entryIndex Selected index. */
  void OnSelectEntry(int entryIndex) override;

  /** @brief Opens the exit confirmation. */
  void OnCancel() override;

private:
  enum class MainMenuState {
    Empty,
    SignedOut,
    SignedInLocal,
    SignedInLive,
  };

  [[nodiscard]] MainMenuState getStateProperty() const;
  void setStateProperty(MainMenuState value);
  void ShowOffer();
  void ExitMessageBoxAccepted(System::Object *sender,
                              const System::EventArgs &e);
  void QuickMatchSession();
  void
  CreateSession(Microsoft::Xna::Framework::Net::NetworkSessionType sessionType);
  void
  FindSession(Microsoft::Xna::Framework::Net::NetworkSessionType sessionType);
  void QuickMatchSearchCompleted(System::Object *sender,
                                 const OperationCompletedEventArgs &e);
  void SessionCreated(System::Object *sender,
                      const OperationCompletedEventArgs &e);
  void QuickMatchSessionJoined(System::Object *sender,
                               const OperationCompletedEventArgs &e);
  void LoadLobbyScreen(
      Microsoft::Xna::Framework::Net::NetworkSession *networkSession);
  void InvitedSessionJoined(System::Object *sender,
                            const OperationCompletedEventArgs &e);
  void FailedMessageBox(System::Object *sender, const System::EventArgs &e);
  void ShowFailure(const std::string &message);

  bool trialMode_{false};
  bool updateState_{false};
  MainMenuState state_{MainMenuState::Empty};
};
} // namespace NetRumble
