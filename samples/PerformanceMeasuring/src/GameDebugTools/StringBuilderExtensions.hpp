// SPDX-License-Identifier: MS-PL
#pragma once

#include <array>
#include <cmath>
#include <limits>
#include <vector>

#include "System/Globalization/CultureInfo.hpp"
#include "System/OverflowException.hpp"
#include "System/Text/StringBuilder.hpp"

namespace PerformanceMeasuring::GameDebugTools
{
    enum class AppendNumberOptions
    {
        None = 0,
        PositiveSign = 1,
        NumberGroup = 2,
    };

    constexpr AppendNumberOptions operator|(AppendNumberOptions left, AppendNumberOptions right)
    {
        return static_cast<AppendNumberOptions>(static_cast<int>(left) | static_cast<int>(right));
    }

    constexpr int operator&(AppendNumberOptions left, AppendNumberOptions right)
    {
        return static_cast<int>(left) & static_cast<int>(right);
    }

    class StringBuilderExtensions
    {
    public:
        static void AppendNumber(System::Text::StringBuilder& builder, int number)
        {
            AppendNumbernternal(builder, number, 0, AppendNumberOptions::None);
        }

        static void AppendNumber(System::Text::StringBuilder& builder, int number,
                                 AppendNumberOptions options)
        {
            AppendNumbernternal(builder, number, 0, options);
        }

        static void AppendNumber(System::Text::StringBuilder& builder, float number)
        {
            AppendNumber(builder, number, 2, AppendNumberOptions::None);
        }

        static void AppendNumber(System::Text::StringBuilder& builder, float number,
                                 AppendNumberOptions options)
        {
            AppendNumber(builder, number, 2, options);
        }

        static void AppendNumber(System::Text::StringBuilder& builder, float number,
                                 int decimalCount, AppendNumberOptions options)
        {
            if (std::isnan(number))
            {
                builder.Append("NaN");
            }
            else if (std::isinf(number) && number < 0.0f)
            {
                builder.Append("-Infinity");
            }
            else if (std::isinf(number))
            {
                builder.Append("+Infinity");
            }
            else
            {
                const int intNumber = static_cast<int>(
                    number * static_cast<float>(std::pow(10.0, decimalCount)) + 0.5f);
                AppendNumbernternal(builder, intNumber, decimalCount, options);
            }
        }

    private:
        static const std::vector<int>& getNumberGroupSizes()
        {
            static const std::vector<int> sizes = [] {
                const auto source = System::Globalization::CultureInfo::getCurrentCultureProperty()
                                        .getNumberFormatProperty().getNumberGroupSizesProperty();
                return std::vector<int>(source.begin(), source.end());
            }();
            return sizes;
        }

        static void AppendNumbernternal(System::Text::StringBuilder& builder, int number,
                                        int decimalCount, AppendNumberOptions options)
        {
            const auto& numberFormat =
                System::Globalization::CultureInfo::getCurrentCultureProperty().getNumberFormatProperty();
            const auto& numberGroupSizes = getNumberGroupSizes();
            std::array<char, 32>& numberString = getNumberString();

            int index = static_cast<int>(numberString.size());
            int decimalPosition = index - decimalCount;
            if (decimalPosition == index)
                decimalPosition = index + 1;

            int numberGroupIndex = 0;
            int numberGroupCount = numberGroupSizes.at(0) + decimalCount;
            const bool showNumberGroup = (options & AppendNumberOptions::NumberGroup) != 0;
            const bool showPositiveSign = (options & AppendNumberOptions::PositiveSign) != 0;

            const bool isNegative = number < 0;
            if (number == std::numeric_limits<int>::min())
                throw System::OverflowException("Negating the minimum integer is invalid.");
            number = std::abs(number);

            do
            {
                if (index == decimalPosition)
                    numberString[static_cast<std::size_t>(--index)] =
                        numberFormat.getNumberDecimalSeparatorProperty().at(0);

                if (--numberGroupCount < 0 && showNumberGroup)
                {
                    numberString[static_cast<std::size_t>(--index)] =
                        numberFormat.getNumberGroupSeparatorProperty().at(0);
                    if (numberGroupIndex < static_cast<int>(numberGroupSizes.size()) - 1)
                        ++numberGroupIndex;
                    numberGroupCount = numberGroupSizes[static_cast<std::size_t>(numberGroupIndex)] - 1;
                }

                numberString[static_cast<std::size_t>(--index)] = static_cast<char>('0' + number % 10);
                number /= 10;
            }
            while (number > 0 || decimalPosition <= index);

            if (isNegative)
                numberString[static_cast<std::size_t>(--index)] =
                    numberFormat.getNegativeSignProperty().at(0);
            else if (showPositiveSign)
                numberString[static_cast<std::size_t>(--index)] =
                    numberFormat.getPositiveSignProperty().at(0);

            while (index < static_cast<int>(numberString.size()))
                builder.Append(numberString[static_cast<std::size_t>(index++)]);
        }

        static std::array<char, 32>& getNumberString()
        {
            static std::array<char, 32> value{};
            return value;
        }
    };
}
