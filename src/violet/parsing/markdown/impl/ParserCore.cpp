#include <iostream>
#include <violet/parsing/Markdown.hpp>

#include <string>
namespace violet {

static void delegateParse(
    std::stringstream& in,
    Markdown::DOMTree* rootTree,
    Markdown::DocumentContext& context
) {
    size_t lastOffset = in.tellg();
    while (in) {
        while (in.peek() == '\n') {
            std::ignore = in.get();
        }

        // the loop condition may be invalidated by the previous while loop, so need to recheck again
        // We also need a redundant check to make sure there is a valid character, since eof doesn't seem to kick in
        // until we do an invalid read?
        // This may be an issue elsewhere as well; need to do further sanity checking on edge cases
        if (!in || in.peek() < 0) {
            break;
        }

        if (!nextMajorMode(in, rootTree, context, true)) {
            throw std::runtime_error("Failed to resolve next mode");
        }

        if (lastOffset == in.tellg()) {
            throw std::runtime_error(
                "Critical: avoided infinite iteration at byte position " + std::to_string(lastOffset)
            );
        }
        lastOffset = in.tellg();
    }
}

std::string Markdown::parse(
    std::stringstream& in,
    const LinkTranslator& linkTranslator
) {
    DOMTree rootTree(NodeType::DocumentRoot);
    DocumentContext context(linkTranslator);

    delegateParse(in, &rootTree, context);

    return stringifyTree(&rootTree, context);
}

Markdown::ContentDeclaration Markdown::parseWithContentPostprocessing(
    std::stringstream& in,
    const LinkTranslator& linkTranslator
) {
    DOMTree rootTree(NodeType::DocumentRoot);
    DocumentContext context(linkTranslator);

    delegateParse(in, &rootTree, context);

    return ContentDeclaration {
        .parsedContents = std::move(
            stringifyTree(&rootTree, context)
        ),
        .tableOfContents = std::move(
            serializeHeaders(rootTree, context)
        ),
    };
}

}
