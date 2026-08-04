#pragma once

#include "violet/parsing/markdown/CommonDefs.hpp"
#include "violet/parsing/markdown/DOMTree.hpp"
#include "violet/parsing/markdown/DocumentContext.hpp"
#include "violet/parsing/markdown/output/ContentDeclaration.hpp"
#include <sstream>
#include <string>

namespace violet::Markdown {


extern std::string stringifyTree(
    const DOMTree* tree,
    DocumentContext& context
);

extern std::string serializeHeaders(
    DOMTree& root,
    DocumentContext& ctx
);

extern std::string parse(
    std::stringstream& in,
    const LinkTranslator& linkTranslator = [](const std::string& in) {
        return in;
    }
);

extern ContentDeclaration parseWithContentPostprocessing(
    std::stringstream& in,
    const LinkTranslator& linkTranslator = [](const std::string& in) {
        return in;
    }
);

}
