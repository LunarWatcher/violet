#include "Escaping.hpp"

void violet::htmlEscape(
    char ch,
    std::stringstream& target
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
        target << "&#" << (int) ch;
        break;
    default:
        target << ch;
    }
}

void violet::urlEncode(
    char ch,
    std::stringstream& target,
    bool includeReserved
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
    if (!includeReserved) {
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
        case '/':
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
    for (auto ch : str) {
        urlEncode(
            ch,
            target,
            includeReserved
        );
    }
}
