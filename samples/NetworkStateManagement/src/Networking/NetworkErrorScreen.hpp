// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// NetworkErrorScreen.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <exception>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "Screens/MessageBoxScreen.hpp"

namespace NetworkStateManagement
{
    /**
     * @brief Specialized message box subclass, used to display network error messages.
     */
    class NetworkErrorScreen final : public MessageBoxScreen
    {
    public:
        /**
         * @brief Constructs an error message box from the specified exception.
         *
         * @param exception The exception a network operation threw.
         */
        explicit NetworkErrorScreen(const std::exception& exception);

        /** @brief Returns the managed type name. @return The type name. */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    private:
        static std::string GetErrorMessage(const std::exception& exception);
    };
}
