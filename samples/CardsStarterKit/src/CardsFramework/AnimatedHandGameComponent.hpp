// SPDX-License-Identifier: MS-PL
#pragma once

// AnimatedHandGameComponent.hpp -- C++ port of UI/AnimatedHandGameComponent.cs
// (XNA 4.0 CardsStarterKit sample).
//
// The original ComponentRemoved/Dispose event path is retained. The C++
// destructor also performs the same idempotent cleanup because collection
// ownership is non-GC and a component can be destroyed during final teardown.

#include <memory>
#include <vector>

#include "Microsoft/Xna/Framework/Vector2.hpp"
#include "Microsoft/Xna/Framework/GameComponentCollectionEventArgs.hpp"

#include "AnimatedCardsGameComponent.hpp"
#include "AnimatedGameComponent.hpp"
#include "CardsGame.hpp"
#include "Hand.hpp"

namespace CardsFramework {

using Microsoft::Xna::Framework::Vector2;

class AnimatedHandGameComponent : public AnimatedGameComponent {
public:
    int Place;
    Hand& HandRef;

    bool IsAnimating() const override {
        for (auto& card : heldAnimatedCards_)
            if (card->IsAnimating())
                return true;
        return false;
    }

    const std::vector<std::shared_ptr<AnimatedCardsGameComponent>>& AnimatedCards() const {
        return heldAnimatedCards_;
    }

    AnimatedHandGameComponent(int place, Hand& hand, CardsGame& cardGame)
        : AnimatedGameComponent(cardGame, nullptr), Place(place), HandRef(hand) {
        receivedCardToken_ = hand.ReceivedCard.Add(
            [this](System::Object*, const CardEventArgs& e) { Hand_ReceivedCard(e); });
        lostCardToken_ = hand.LostCard.Add(
            [this](System::Object*, const CardEventArgs& e) { Hand_LostCard(e); });

        CurrentPosition = (place == -1) ? CardGame->GameTableInstance->DealerPosition
                                        : (*CardGame->GameTableInstance)[place];

        for (int cardIndex = 0; cardIndex < hand.Count(); cardIndex++) {
            auto animatedCardGameComponent = cardGame.AddComponent<AnimatedCardsGameComponent>(&hand[cardIndex], cardGame);
            animatedCardGameComponent->CurrentPosition = CurrentPosition + Vector2(30.0f * cardIndex, 0.0f);
            heldAnimatedCards_.push_back(animatedCardGameComponent);
        }

        componentRemovedToken_ = getGameProperty().getComponentsProperty().ComponentRemoved.Add(
            [this](System::Object* sender,
                   const Microsoft::Xna::Framework::GameComponentCollectionEventArgs& e) {
                Components_ComponentRemoved(sender, e);
            });
    }

    ~AnimatedHandGameComponent() override {
        UnsubscribeHandEvents();
        getGameProperty().getComponentsProperty().ComponentRemoved.Remove(componentRemovedToken_);
    }

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CardsFramework.AnimatedHandGameComponent";
        return name;
    }

    // Arranges the hand's animated cards' positions.
    void Update(GameTime& gameTime) override {
        for (size_t i = 0; i < heldAnimatedCards_.size(); i++) {
            if (!heldAnimatedCards_[i]->IsAnimating()) {
                heldAnimatedCards_[i]->CurrentPosition = CurrentPosition + GetCardRelativePosition((int)i);
            }
        }
        AnimatedGameComponent::Update(gameTime);
    }

    // Gets the card's offset from the hand position according to its index
    // in the hand. Overridden per game (e.g. fanned player hands).
    virtual Vector2 GetCardRelativePosition(int cardLocationInHand) const {
        (void)cardLocationInHand;
        return Vector2::Zero;
    }

    // Finds the index of a specified card in the hand, or -1 if not found.
    int GetCardLocationInHand(const TraditionalCard* card) const {
        for (size_t i = 0; i < heldAnimatedCards_.size(); i++)
            if (heldAnimatedCards_[i]->Card == card)
                return (int)i;
        return -1;
    }

    std::shared_ptr<AnimatedCardsGameComponent> GetCardGameComponent(const TraditionalCard* card) const {
        int location = GetCardLocationInHand(card);
        return GetCardGameComponent(location);
    }

    std::shared_ptr<AnimatedCardsGameComponent> GetCardGameComponent(int location) const {
        if (location == -1 || location >= (int)heldAnimatedCards_.size())
            return nullptr;
        return heldAnimatedCards_[location];
    }

    TimeSpan EstimatedTimeForAnimationsCompletion() const override {
        TimeSpan result = TimeSpan::Zero;
        if (IsAnimating()) {
            for (auto& card : heldAnimatedCards_) {
                if (card->EstimatedTimeForAnimationsCompletion() > result)
                    result = card->EstimatedTimeForAnimationsCompletion();
            }
        }
        return result;
    }

private:
    void Components_ComponentRemoved(
        System::Object* sender,
        const Microsoft::Xna::Framework::GameComponentCollectionEventArgs& e) {
        (void)sender;
        if (e.getGameComponentProperty() == this)
            Microsoft::Xna::Framework::GameComponent::Dispose();
    }

    void Hand_LostCard(const CardEventArgs& e) {
        for (size_t i = 0; i < heldAnimatedCards_.size(); i++) {
            if (heldAnimatedCards_[i]->Card == e.Card) {
                CardGame->RemoveComponent(heldAnimatedCards_[i]);
                heldAnimatedCards_.erase(heldAnimatedCards_.begin() + i);
                return;
            }
        }
    }

    void Hand_ReceivedCard(const CardEventArgs& e) {
        auto animatedCardGameComponent = CardGame->AddComponent<AnimatedCardsGameComponent>(e.Card, *CardGame);
        animatedCardGameComponent->setVisibleProperty(false);
        heldAnimatedCards_.push_back(animatedCardGameComponent);
    }

    std::vector<std::shared_ptr<AnimatedCardsGameComponent>> heldAnimatedCards_;
    System::EventHandler<CardEventArgs>::Token receivedCardToken_;
    System::EventHandler<CardEventArgs>::Token lostCardToken_;
    System::EventHandler<Microsoft::Xna::Framework::GameComponentCollectionEventArgs>::Token componentRemovedToken_;
    bool unsubscribed_ = false;

protected:
    void Dispose(bool disposing) override {
        if (disposing)
            UnsubscribeHandEvents();
        AnimatedGameComponent::Dispose(disposing);
    }

private:
    void UnsubscribeHandEvents() {
        if (unsubscribed_)
            return;
        HandRef.ReceivedCard.Remove(receivedCardToken_);
        HandRef.LostCard.Remove(lostCardToken_);
        unsubscribed_ = true;
    }
};

} // namespace CardsFramework
