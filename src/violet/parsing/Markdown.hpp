#pragma once

#include "violet/parsing/markdown/CommonDefs.hpp"
#include "violet/parsing/markdown/DOMTree.hpp"
#include "violet/parsing/markdown/DocumentContext.hpp"
#include "violet/parsing/markdown/output/ContentDeclaration.hpp"
#include <sstream>
#include <string>

namespace violet::Markdown {

const inline LinkTranslator defaultTranslator = [](const auto& in) { return in; };

extern std::string stringifyTree(
    const DOMTree* tree,
    DocumentContext& context
);

extern std::string serializeHeaders(
    DOMTree& root,
    DocumentContext& ctx,
    int minToCLevel
);

extern std::string parse(
    std::stringstream& in,
    const LinkTranslator& linkTranslator = [](const std::string& in) {
        return in;
    }
);

extern ContentDeclaration parseWithContentPostprocessing(
    std::stringstream& in,
    const LinkTranslator& linkTranslator = defaultTranslator,
    int minToCLevel = 1
);

}
