#include "violet/parsing/Markdown.hpp"
#include "violet/parsing/markdown/ParseAPI.hpp"
#include "violet/parsing/Escaping.hpp"
#include "violet/parsing/markdown/DOMTree.hpp"
#include "violet/parsing/markdown/DocumentContext.hpp"
#include "violet/parsing/markdown/ElementaryNodes.hpp"
#include "violet/parsing/markdown/ContextConsumingNodes.hpp"
#include "violet/parsing/markdown/ContextProvidingNodes.hpp"
#include "violet/util/LockStreamPos.hpp"

#include <sstream>

namespace violet {

void Markdown::parseCodeBlockContent(
    std::stringstream& in,
    CodeNode* out,
    DocumentContext&
) {
    char ch;
    std::stringstream content;
    // Inline code is pre-prepared by its parent statement. Only code blocks need to prepare the stream.
    if (out->type == NodeType::CodeBlock) {
        prepareStream(in, out);
    }
    while (in >> std::noskipws >> ch) {
        if (
            ch == '\n'
            && out->type == NodeType::InlineCode
        ) {
            // Node is invalid
            throw SyntaxError(
                "Inline code not terminated before newline",
                in.tellg()
            );
        } else if (ch == '\n' && content.tellp() == 0) {
            continue;
        } else {
            if (
                ch == '\n' && in.peek() != '\n'
                && !prepareStream(in, out)
            ) {
                throw SyntaxError(
                    "Invalid indent in code block",
                    in.tellg()
                );
            }
            if (ch == '\n' || ch == '`') {
                size_t boundry = (size_t) ch == '`';
                while (in.peek() == '`') {
                    std::ignore = in.get();
                    ++boundry;
                }

                if (boundry == out->boundrySize) {
                    // This consumes trailing newlines in proper code blocks, i.e. after the final backtick. Also
                    // validates the markdown
                    if (
                        in
                        && out->type == NodeType::CodeBlock
                        && in >> std::noskipws >> ch
                        && ch != '\n'
                    ) {
                        throw SyntaxError(
                            std::format(
                                "Code block constraint violated: must end with a newline, found n={} ({})",
                                (int) ch, ch
                            ),
                            in.tellg()
                        );
                    }
                    break;
                }

                if (ch == '\n') {
                    content << "\n";
                }
                for (size_t i = 0; i < boundry; ++i) {
                    content << "`";
                }
                continue;
            }
            // TODO: each line could (as far as I can tell) be rendered as a <span> within the code, though this will
            // require per-line rendering and adding a special rendering thing when it's converted to HTML
            // Though I do suspect it should be a config option, so not doing it now
            // (once ::first-line gets expanded, this could probably be omitted)
            htmlEscape(ch, content);
        }
    }
    commitContentNode(content, out);
}

void Markdown::parseCodeBlock(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
) {
    prepareStream(in, out, true);
    size_t boundry = 0;
    while (in.peek() == '`') {
        std::ignore = in.get();
        ++boundry;
    }

    std::stringstream lang;

    if (in.peek() != '\n') {
        char ch;
        while (in >> std::noskipws >> ch) {
            if (ch == '\n') {
                break;
            }
            if (ch != '"') {
                lang << ch;
            }
        }
    } else {
        in.get(); // Consume newline
    }

    auto node = new CodeNode(
        boundry,
        false,
        lang.str()
    );
    out->addChild(node);

    parseCodeBlockContent(in, node, context);
}


}
