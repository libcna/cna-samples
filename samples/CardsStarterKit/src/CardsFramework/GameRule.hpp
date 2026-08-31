// SPDX-License-Identifier: MS-PL
#pragma once

// GameRule.hpp -- C++ port of Rules/GameRule.cs (XNA 4.0 CardsStarterKit sample).

#include "System/EventArgs.hpp"
#include "System/EventHandler.hpp"
#include "System/Object.hpp"
#include "CNA/CNAHelper.hpp"

namespace CardsFramework {

// Represents a rule in a card game. Inherit and implement Check().
class GameRule : public System::Object {
public:
    // An event which triggers when the rule conditions are matched.
    System::EventHandler<System::EventArgs> RuleMatch;

    virtual ~GameRule() = default;

    CNAEXT [[nodiscard]] const std::string& GetTypeName() const override {
        static const std::string name = "CardsFramework.GameRule";
        return name;
    }

    // Checks whether the rule conditions are met. Should call FireRuleMatch().
    virtual void Check() = 0;

protected:
    void FireRuleMatch(const System::EventArgs& e) { RuleMatch.Raise(this, e); }
};

} // namespace CardsFramework
