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
            target << ch;
            return;
        }
    }

    target << std::hex << (int) ch << std::dec;
}
