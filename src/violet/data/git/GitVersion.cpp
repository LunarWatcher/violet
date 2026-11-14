#include "GitVersion.hpp"

extern std::string_view violet::GitVersion() noexcept {
    return VIOLET_VERSION;
}
