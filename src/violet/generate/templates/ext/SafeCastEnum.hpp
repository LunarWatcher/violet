#pragma once

#include <optional>
#include <type_traits>
namespace violet {

/**
 * Safe cast an int to an enum.
 * The enum MUST have the Start and End values. Start and End passed to this function returns an empty optional, similar
 * to illegal values.
 *
 * \returns An empty optional if \p value is out of range, or a value of the template type T
 */
template <class T>
std::optional<T> safeCastEnum(int value) {
    static_assert(
        std::is_enum_v<T>,
        "safeCastEnum requires an enum type"
    );
    if (value <= static_cast<int>(T::Start) || value >= static_cast<int>(T::End)) {
        return std::nullopt;
    }
    return static_cast<T>(value);
}

}
