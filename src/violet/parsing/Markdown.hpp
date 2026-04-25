#pragma once

#include "violet/parsing/markdown/DOMTree.hpp"
#include "violet/parsing/markdown/DocumentContext.hpp"
#include "violet/parsing/markdown/ElementaryNodes.hpp"
#include <functional>
#include <sstream>
#include <string>

namespace violet::Markdown {

// TODO: pass stringstream& instead?
using LinkTranslator = std::function<std::string(const std::string& rawUrl)>;

/**
 * Prepares the stream by removing indent levels to pretend like it's being handled at the root level. Only takes effect
 * if the DOMTree path includes quotes, lists, or other things that affect the prefix of the node.  
 *
 * \returns Whether or not the indent is still correct. If false, the current node has ended due to changes in a parent
 *          node. This may also turn out to be due to invalid markup, but this is not the concern of the node parsing
 *          functions.
 */
extern bool prepareStream(
    std::stringstream& in,
    DOMTree* tree,
    bool includeBullets = true
);

extern void insertCharacterEscaped(
    char ch,
    std::stringstream& target
);

extern NodeType resolveMajorMode(
    std::stringstream& in,
    DOMTree* tree,
    bool bulletBoundries = false
);

extern bool nextMajorMode(
    std::stringstream& in,
    DOMTree* tree,
    DocumentContext& context,
    bool bulletBoundries = true
);

extern void parseCodeBlockContent(
    std::stringstream& in,
    CodeNode* out,
    DocumentContext& context
);

extern void parseAnchorDef(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
);

extern void parseFootnoteDef(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
);

extern void parseParagraphContent(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
);

extern void parseParagraph(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
);

extern void parseUnorderedList(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
);

extern void parseOrderedList(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
);

extern void parseQuote(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
);

extern void parseCallout(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
);

extern void parseCodeBlock(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
);

extern void parseHeader(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
);


extern std::string stringifyTree(
    const DOMTree& tree,
    DocumentContext& context
);

extern std::string parse(
    std::stringstream& in,
    const LinkTranslator& translator = [](const std::string& in) {
        return in;
    }
);

}
