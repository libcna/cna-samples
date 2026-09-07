#pragma once

// Int32Range.hpp -- C++ port of RolePlayingGameData/Data/Int32Range.cs.

#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Content/ContentReader.hpp"
#include "Microsoft/Xna/Framework/Content/ContentTypeReader.hpp"
#include "System/Random.hpp"
#include "System/Int32.hpp"

namespace RolePlayingGameData {

// Defines a range of values, useful for generating values in that range.
struct Int32Range {
    int Minimum = 0;
    int Maximum = 0;

    Int32Range() = default;
    Int32Range(int minimum, int maximum) : Minimum(minimum), Maximum(maximum) {}

    int Average() const { return Minimum + Range() / 2; }
    int Range() const { return Maximum - Minimum; }

    // Generate a random value between the minimum and maximum, inclusively.
    int GenerateValue(System::Random& random) const { return random.Next(Minimum, Maximum); }

    static Int32Range Add(const Int32Range& a, const Int32Range& b) {
        return Int32Range(a.Minimum + b.Minimum, a.Maximum + b.Maximum);
    }
    static Int32Range Subtract(const Int32Range& a, const Int32Range& b) {
        return Int32Range(a.Minimum - b.Minimum, a.Maximum - b.Maximum);
    }
    static Int32Range Add(const Int32Range& r, int amount) {
        return Int32Range(r.Minimum + amount, r.Maximum + amount);
    }
    static Int32Range Subtract(const Int32Range& r, int amount) {
        return Int32Range(r.Minimum - amount, r.Maximum - amount);
    }

    friend Int32Range operator+(const Int32Range& a, const Int32Range& b) { return Add(a, b); }
    friend Int32Range operator-(const Int32Range& a, const Int32Range& b) { return Subtract(a, b); }
    friend Int32Range operator+(const Int32Range& r, int amount) { return Add(r, amount); }
    friend Int32Range operator-(const Int32Range& r, int amount) { return Subtract(r, amount); }
    Int32Range& operator+=(const Int32Range& other) { *this = *this + other; return *this; }
    Int32Range& operator-=(const Int32Range& other) { *this = *this - other; return *this; }
    Int32Range& operator+=(int amount) { *this = *this + amount; return *this; }
    Int32Range& operator-=(int amount) { *this = *this - amount; return *this; }

    std::string ToString() const {
        return "(" + System::Int32::ToString(Minimum) + "," + System::Int32::ToString(Maximum) + ")";
    }
};

// Read an Int32Range object from the content pipeline.
class Int32RangeReader final
    : public Microsoft::Xna::Framework::Content::ContentTypeReader<Int32Range> {
public:
    Int32RangeReader()
        : Microsoft::Xna::Framework::Content::ContentTypeReader<Int32Range>(
              "RolePlayingGameData.Int32Range") {}

protected:
    Int32Range Read(Microsoft::Xna::Framework::Content::ContentReader& input,
                    std::optional<Int32Range> existingInstance) override {
        Int32Range output = existingInstance.value_or(Int32Range());

        output.Minimum = static_cast<int>(input.ReadInt32());
        output.Maximum = static_cast<int>(input.ReadInt32());

        return output;
    }
};

} // namespace RolePlayingGameData
