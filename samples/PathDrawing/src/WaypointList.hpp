// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// WaypointList.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "System/Collections/Generic/Queue.hpp"

namespace PathDrawing
{
    using Microsoft::Xna::Framework::Vector2;

    /**
     * @brief WaypointList is a queue of locations that our Tank should drive towards.
     */
    class WaypointList : public System::Collections::Generic::Queue<Vector2>
    {
    public:
        /**
         * @brief Gets the position in the queue at the given index.
         *
         * @param index Zero-based position in the queue.
         * @return The waypoint at that position, or Vector2::Zero when the index is out of range.
         */
        [[nodiscard]] Vector2 getItem(int index) const
        {
            Vector2 value = Vector2::Zero;

            // We use a range-based loop because a Queue<T> doesn't have any way to index
            for (const Vector2& v : *this)
            {
                index--;
                if (index < 0)
                {
                    value = v;
                    break;
                }
            }

            return value;
        }
    };
}
