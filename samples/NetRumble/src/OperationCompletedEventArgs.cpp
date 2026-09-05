// SPDX-License-Identifier: MS-PL
#include "OperationCompletedEventArgs.hpp"

namespace NetRumble {
OperationCompletedEventArgs::OperationCompletedEventArgs(
    System::IAsyncResult *asyncResult)
    : asyncResult_(asyncResult) {}

System::IAsyncResult *
OperationCompletedEventArgs::getAsyncResultProperty() const {
  return asyncResult_;
}

void OperationCompletedEventArgs::setAsyncResultProperty(
    System::IAsyncResult *value) {
  asyncResult_ = value;
}
} // namespace NetRumble
