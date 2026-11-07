#pragma once

#include "catch2/catch_test_macros.hpp"
#include "violet/parsing/Escaping.hpp"
#include "violet/parsing/LinkTranslate.hpp"
#include "violet/parsing/Markdown.hpp"
#include <string>
#include <format>
#include <sstream>
#include <ranges>

namespace tests {

struct MarkdownFixture {

    std::string checklist(bool checked) const {
        std::stringstream ss;
        ss << R"(<input class="violet-checklist" type="checkbox" disabled)";
        if (checked) {
            ss << " checked";
        }
        ss << "/>";
        return ss.str();
    }

    std::string url(const std::string& content, const std::string& url) const {

        return std::format(
            R"(<a href="{}">{}</a>)",
            url,
            content
        );
    }

    std::string htmlEscape(const std::string& content) const {
        std::stringstream ss;
        for (auto ch : content) {
            violet::htmlEscape(ch, ss);
        }

        return ss.str();
    }

    std::string paragraph(const std::string& content) const {
        return std::format("<p>{}</p>", content);
    }
    std::string paragraph(const std::vector<std::string>& content) const {
        return std::format(
            "<p>{:s}</p>",
            content | std::views::join
        );
    }

    std::string img(const std::string& alt, const std::string& link) const {
        return std::format(R"(<img src="{}" alt="{}"/>)", link, alt);
    }

    std::string render(
        const std::string& content,
        const std::string& prefix = "/",
        const std::string& relPath = "README.md"
    ) const {
        if (relPath.starts_with('/')) {
            FAIL("Test developer error: render() got non-relative relPath: " + relPath);
        }
        std::stringstream ss;
        ss << content;
        return violet::Markdown::parse(
            ss,
            std::bind(
                violet::projectBasedTranslator,
                prefix,
                relPath,
                std::nullopt,
                std::placeholders::_1
            )
        );
    }
};

}
