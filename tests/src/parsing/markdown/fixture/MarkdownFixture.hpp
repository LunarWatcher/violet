#pragma once

#include <string>
#include <sstream>

namespace tests {

struct MarkdownFixture {

    std::string checklist(bool checked) {
        std::stringstream ss;
        ss << R"(<input class="violet-checklist" type="checkbox" disabled)";
        if (checked) {
            ss << " checked";
        }
        ss << "/>";
        return ss.str();
    }
};

}
