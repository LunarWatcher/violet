#include "Escaping.hpp"

void violet::htmlEscape(
    char ch,
    std::stringstream& target,
    bool extended
) {
    switch (ch) {
    case '&':
        target << "&amp;";
        break;
    case '<':
        target << "&lt;";
        break;
    case '>':
        target << "&gt;";
        break;
    case '{':
    case '}':
    case '#':
        target << "&#" << (int) ch << ";";
        break;
    case '"':
    case '\'':
        if (extended) {
            target << "&#" << (int) ch << ";";
            break;
        }
        [[fallthrough]];
    default:
        target << ch;
    }
}
void violet::urlEncode(
    char ch,
    std::stringstream& target,
    bool includeReserved
) {
    urlEncode(
        ch,
        target,
        includeReserved,
        includeReserved
    );
}

void violet::urlEncode(
    char ch,
    std::stringstream& target,
    bool includeReserved,
    bool includeSlash
) {
    if (
        (ch >= 'A' && ch <= 'Z')
        || (ch >= 'a' && ch <= 'z')
        || (ch >= '0' && ch <= '9')
        || ch == '-'
        || ch == '.'
        || ch == '_'
        || ch == '~'
    ) {
        target << ch;
        return;
    }
    if (!includeSlash && ch == '/') {
        target << ch;
        return;
    } else if (!includeReserved) {
        switch (ch) {
        case '!':
        case '#':
        case '$':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case ':':
        case ';':
        case '=':
        case '?':
        case '@':
        case '[':
        case ']':

        // Not technically reserved, but we're excluding it to avoid double-encoding
        case '%':
            target << ch;
            return;
        default:;
        }
    }

    target << std::hex
           << "%"
           << (int) ch
           << std::dec;
}

void violet::urlEncode(
    const std::string& str,
    std::stringstream& target,
    bool includeReserved
) {
    urlEncode(
        str,
        target,
        includeReserved,
        includeReserved
    );
}

void violet::urlEncode(
    const std::string& str,
    std::stringstream& target,
    bool includeReserved,
    bool includeSlash
) {
    for (auto ch : str) {
        urlEncode(
            ch,
            target,
            includeReserved,
            includeSlash
        );
    }
}
