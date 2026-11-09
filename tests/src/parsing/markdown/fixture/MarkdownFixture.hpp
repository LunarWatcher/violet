#pragma once

#include "catch2/catch_test_macros.hpp"
#include "violet/conf/MarkdownConfig.hpp"
#include "violet/parsing/Escaping.hpp"
#include "violet/parsing/LinkTranslate.hpp"
#include "violet/parsing/Markdown.hpp"
#include <string>
#include <format>
#include <sstream>
#include <ranges>

namespace tests {

struct Footnote {
    std::string renderedContent;
    size_t usages = 1;
};

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

    std::string code(const std::string& content, const std::string& language = "") {
        return std::format(
            "<pre><code{}>{}</code></pre>",
            language.empty() ? "" : std::format(" class=\"language-{}\"", language),
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

    std::string fnref(size_t fnNum, size_t fnRefNum) {
        return std::format(
            R"(<sup><a href="#fn-{0}" id="fn-{0}-r{1}">[{0}]</a></sup>)",
            fnNum,
            fnRefNum
        );
    }

    std::string bold(const std::string& content) {
        return std::format("<strong>{}</strong>", content);
    }
    
    std::string footnotes(
        const std::vector<Footnote>& fns,
        bool withHeaders = true
    ) {
        std::stringstream ss;
        ss << R"(<div id="violet-footnotes">)";
        if (withHeaders) {
            ss << R"(<h2>Footnotes</h2>)";
        }

        ss << "<ol>";
        size_t fnIdx = 0;
        for (auto& fn : fns)  {
            ++fnIdx;
            ss << std::format(R"(<li id="fn-{}">)", fnIdx);
            ss << fn.renderedContent;
            ss << R"(<span class="footnote-backref">)"
               << (withHeaders ? "Usages: " : "↩ ");
            for (size_t usageIdx = 1; usageIdx <= fn.usages; ++usageIdx) {
                if (usageIdx != 1) {
                    ss << ", ";
                }
                ss << std::format(
                    R"(<a href="#fn-{0}-r{1}">[{1}]</a>)",
                    fnIdx,
                    usageIdx
                );
            }
            ss <<  R"(</span></li>)";
        }
        ss << R"(</ol></div>)";
        return ss.str();
    }

    std::string render(
        const std::string& content,
        const std::string& prefix = "/",
        const std::string& relPath = "README.md"
    ) const {
        return render(content, {}, prefix, relPath);
    }

    std::string render(
        const std::string& content,
        const violet::MarkdownConfig& config,
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
            ),
            config
        );
    }
};

}
