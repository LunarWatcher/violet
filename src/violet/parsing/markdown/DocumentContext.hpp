#pragma once

#include "violet/parsing/markdown/CommonDefs.hpp"
#include "violet/parsing/markdown/ContextProvidingNodes.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace violet::Markdown {

struct DocumentContext {
    std::unordered_map<std::string, std::string> externalLinkMap;
    std::vector<std::string> usedFootnotes;
    std::unordered_map<std::string, size_t> footnoteUsageCounts;
    std::unordered_map<std::string, FootnoteDefNode*> footnotes;
    std::unordered_set<std::string> titleIds;

    std::optional<std::string> footnoteHeader;

    LinkTranslator linkTranslator;

    DocumentContext(
        const LinkTranslator& linkTranslator
    ) : linkTranslator(linkTranslator) {}

    ~DocumentContext() {
        for (auto& [_, node] : footnotes) {
            delete node;
        }
    }

    std::string idify(const std::string& content);
};


}
