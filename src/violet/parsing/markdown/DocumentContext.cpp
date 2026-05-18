#include "DocumentContext.hpp"

#include <sstream>

namespace violet::Markdown {

std::string DocumentContext::idify(const std::string& content) {
    std::stringstream ss;

    for (auto ch : content) {
        if (ch >= 'A' && ch <= 'Z') {
            ss << ((char) ('a' + (ch - 'A')));
        } else if ((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9')) {
            ss << ch;
        } else {
            switch (ch) { // NOLINT(bugprone-switch-missing-default-case)
            case '_':
            case '-':
                ss << ch;
                break;
            case ' ':
                ss << "-";
            }
        }
    }

    std::string identified = ss.str();
    std::string real = identified;
    size_t i = 0;
    while (real.empty() || this->titleIds.contains(real)) {
        real = identified + "-" + std::to_string(++i);
    }

    this->titleIds.insert(real);
    return real;
}

}
