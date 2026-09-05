// SPDX-License-Identifier: MS-PL
#pragma once

#include "System/EventArgs.hpp"

namespace System {
class IAsyncResult;
}

namespace NetRumble {
/** @brief Carries the result of an asynchronous network operation. */
class OperationCompletedEventArgs final : public System::EventArgs {
public:
  /** @brief Constructs the event data. @param asyncResult Completed operation.
   */
  explicit OperationCompletedEventArgs(System::IAsyncResult *asyncResult);

  /** @brief Gets the completed operation. @return Async result. */
  [[nodiscard]] System::IAsyncResult *getAsyncResultProperty() const;

  /** @brief Sets the completed operation. @param value Async result. */
  void setAsyncResultProperty(System::IAsyncResult *value);

private:
  System::IAsyncResult *asyncResult_;
};
} // namespace NetRumble
