// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// OperationCompletedEventArgs.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "System/EventArgs.hpp"
#include "System/IAsyncResult.hpp"

namespace NetworkStateManagement
{
    /**
     * @brief Custom EventArgs class used by the NetworkBusyScreen.OperationCompleted event.
     */
    class OperationCompletedEventArgs final : public System::EventArgs
    {
    public:
        /**
         * @brief Constructs a new event arguments class.
         *
         * @param asyncResult The completed asynchronous operation.
         */
        explicit OperationCompletedEventArgs(System::IAsyncResult* asyncResult) : asyncResult_(asyncResult) {}

        /** @brief Gets the IAsyncResult associated with the network operation that has just completed. @return The result. */
        [[nodiscard]] System::IAsyncResult* getAsyncResultProperty() const { return asyncResult_; }
        /** @brief Sets the IAsyncResult. @param value The result. */
        void setAsyncResultProperty(System::IAsyncResult* value) { asyncResult_ = value; }

    private:
        System::IAsyncResult* asyncResult_;
    };
}
