// SPDX-License-Identifier: MS-PL
#pragma once
#include <algorithm>
#include <vector>
namespace NetRumble {
/** @brief Vector-like collection whose removals are applied after an update
 * pass. */
template <class T> class BatchRemovalCollection : public std::vector<T> {
public:
  /** @brief Creates an empty collection with no pending removals. */
  BatchRemovalCollection() = default;
  /** @brief Adds an item. @param item Item to add. */ void Add(const T &item) {
    this->push_back(item);
  }
  /** @brief Queues an item for removal. @param item Item to remove. */ void
  QueuePendingRemoval(const T &item) {
    pending_.push_back(item);
  }
  /** @brief Applies all queued removals. */ void ApplyPendingRemovals() {
    for (const auto &x : pending_) {
      const auto item = std::find(this->begin(), this->end(), x);
      if (item != this->end())
        this->erase(item);
    }
    pending_.clear();
  }

private:
  std::vector<T> pending_;
};
} // namespace NetRumble
