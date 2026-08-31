// SPDX-License-Identifier: MS-PL
#pragma once

// Hand.hpp -- C++ port of Cards/Hand.cs (XNA 4.0 CardsStarterKit sample).
// Represents a hand of cards held by a player, dealer, or the game table.
// A Hand is a CardPacket that may also receive cards from any CardPacket or
// another Hand.

#include <memory>

#include "CardPacket.hpp"
#include "TraditionalCard.hpp"

namespace CardsFramework {

class Hand : public CardPacket {
public:
    // An event which triggers when a card is added to the hand.
    System::EventHandler<CardEventArgs> ReceivedCard;

    Hand() : CardPacket() {}

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CardsFramework.Hand";
        return name;
    }

    // Adds the specified card to the hand, as the last card of the hand.
    // `internal` in the original -- CardsFramework-only callers here too.
    void Add(std::unique_ptr<TraditionalCard> card) {
        CardEventArgs args;
        args.Card = card.get();
        cards_.push_back(std::move(card));
        ReceivedCard.Raise(this, args);
    }

    // Adds all specified cards, preserving their order and without raising
    // ReceivedCard, as in the original IEnumerable overload.
    void Add(std::vector<std::unique_ptr<TraditionalCard>> cards) {
        for (auto& card : cards)
            cards_.push_back(std::move(card));
    }
};

// ---- Out-of-line definitions needing both CardPacket and Hand complete ----

inline void TraditionalCard::MoveToHand(Hand& hand) {
    std::unique_ptr<TraditionalCard> self = HoldingCardCollection->Remove(this);
    HoldingCardCollection = &hand;
    hand.Add(std::move(self));
}

inline TraditionalCard& CardPacket::DealCardToHand(Hand& destinationHand) {
    TraditionalCard* firstCard = cards_.front().get();
    firstCard->MoveToHand(destinationHand);
    return *firstCard;
}

inline std::vector<TraditionalCard*> CardPacket::DealCardsToHand(Hand& destinationHand, int count) {
    std::vector<TraditionalCard*> dealtCards;
    dealtCards.reserve(static_cast<std::size_t>(count));
    for (int cardIndex = 0; cardIndex < count; cardIndex++) {
        dealtCards.push_back(&DealCardToHand(destinationHand));
    }
    return dealtCards;
}

} // namespace CardsFramework
