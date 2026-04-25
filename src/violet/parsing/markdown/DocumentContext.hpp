#pragma once

#include "violet/parsing/markdown/ContextProvidingNodes.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace violet::Markdown {

struct DocumentContext {
    std::unordered_map<std::string, std::string> externalLinkMap;
    std::vector<std::string> usedFootnotes;
    std::unordered_map<std::string, size_t> footnoteUsageCounts;
    std::unordered_map<std::string, FootnoteDefNode*> footnotes;

    ~DocumentContext() {
        for (auto& [_, node] : footnotes) {
            delete node;
        }
    }
};


}
