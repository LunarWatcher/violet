#pragma once

#include "violet/parsing/Escaping.hpp"
#include "violet/parsing/Markdown.hpp"
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

    std::string url(const std::string& content, const std::string& url) {
        std::stringstream ss;
        for (auto ch : content) {
            violet::htmlEscape(ch, ss);
        }

        auto actualContent = ss.str();

        return std::format(
            R"(<a href="{}">{}</a>)",
            url,
            actualContent
        );
    }
    std::string paragraph(const std::string& content) {
        return std::format("<p>{}</p>", content);
    }

    std::string render(const std::string& content) {
        std::stringstream ss;
        ss << content;
        return violet::Markdown::parse(ss);
    }
};

}
