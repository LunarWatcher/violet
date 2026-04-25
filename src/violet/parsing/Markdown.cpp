#include "Markdown.hpp"
#include "violet/exceptions/SyntaxError.hpp"
#include "violet/exceptions/TracedException.hpp"
#include "violet/parsing/Escaping.hpp"
#include "violet/parsing/LinkTranslate.hpp"
#include <ios>
#include <iostream>
#include <stdexcept>

#include "markdown/DOMTree.hpp"
#include "markdown/DocumentContext.hpp"
#include "markdown/ElementaryNodes.hpp"
#include "markdown/ContextConsumingNodes.hpp"
#include "markdown/ContextProvidingNodes.hpp"


namespace violet {


Markdown::NodeType Markdown::resolveMajorMode(
    std::stringstream& in,
    DOMTree* tree,
    bool bulletBoundries
) {
    thread_local std::array<char, 3> next;

    size_t offset = in.tellg();
    NodeType mode;

    if (!prepareStream(in, tree, bulletBoundries)) {
        return NodeType::BlockEnd;
    }

    if (in.peek() == '#') {
        mode = NodeType::Header;
    } else if (in.peek() == '>') {
        std::ignore = in.get();
        while (in.peek() == ' ') {
            std::ignore = in.get();
        }
        if (in.peek() == '[') {
            std::ignore = in.get();
            if (in.peek() == '!') {
                mode = NodeType::Callout;
                goto done;
            }
        }
        // Quote
        mode = NodeType::Quote;
    } else if (in.peek() == '-' || in.peek() == '*') {
        // Bullet list
        std::ignore = in.get();
        if (in.peek() == ' ') {
            mode = NodeType::UnorderedList;
        } else {
            mode = NodeType::Paragraph;
        }
    // } else if (in.peek() == '\n') { // Consume and ignore empty lines not consumed by previous modes
    //     std::ignore = in.get();
    //     // TODO: if this causes stackoverflows, add another meta-mode so the iterative approach can continue to be used
    //     return resolveMajorMode(in, tree, true);
    } else {
        // Paragraph or numbered list or code block
        if (in.peek() != ' ') {
            int c;
            if (in >> c) {
                // The start of the line is a number. Check if the next char is a `.`
                if (in.peek() == '.') {
                    // Numbered list
                    mode = NodeType::OrderedList;
                    goto done;
                }
                // Otherwise, paragraph
            }
            in.clear();
            if (in.peek() == '`') {
                if (in.readsome(next.data(), next.size())
                    && std::string_view{next} == "```") {
                    // Code block
                    // TODO: handle paragraph lines that start with ``` (pretty sure that's legal under the markdown
                    // spec? It's at least legal to have inline codeblocks that have more than one backtick for escaping
                    // nested backticks)
                    mode = NodeType::CodeBlock;
                    goto done;
                }
            } else if (in.peek() == '[') {
                // Possible anchor, footnote def, or callout def
                std::ignore = in.get();
                if (in.peek() == '!') {
                    goto paragraph;
                }
                auto isFootnote = in.peek() == '^';
                if (isFootnote) {
                    std::ignore = in.get();
                }
                NodeType setMode = isFootnote ? NodeType::FootnoteDef : NodeType::AnchorDef;

                char ch;
                while (in && in >> std::noskipws >> ch) {
                    if (ch == '\n' && ch == ' ') {
                        break;
                    } else if (ch == ']') {
                        // if no :, this is just a paragraph that starts with an [urlref] inline url
                        if (in.peek() == ':') {
                            mode = setMode;
                            goto done;
                        }
                        break;
                    }
                }
            }
        }
    paragraph:
        mode = NodeType::Paragraph;
    }
 done:
    // Reset the stream so the parsers can handle it properly
    in.seekg(offset);
    in.clear();
    return mode;
}

void Markdown::commitContentNode(
    std::stringstream& content,
    Markdown::DOMTree* out
) {
    if (content.tellp() > 0) {
        out->addChild(
            new Markdown::ContentNode(
                std::move(content.str())
            )
        );
        content = {};
    }
}

bool Markdown::prepareStream(
    std::stringstream& in,
    DOMTree* tree,
    bool includeBullets
) {
    thread_local std::array<char, 2> buff;
    size_t start = in.tellg();

    // if (in.peek() == '\n') {
    //     return true;
    // }

    DOMTree* curr = tree;
    std::vector<NodeType> types;
    while (curr != nullptr) {
        switch (curr->type) {
        case NodeType::Quote:
        case NodeType::Callout:
        case NodeType::UnorderedList:
        case NodeType::OrderedList:
        case NodeType::FootnoteDef:
            types.push_back(curr->type);
            break;
        default:
            break;
        }
        curr = curr->parent;
    }

    if (!types.empty()) {
        for (auto it = types.rbegin(); it != types.rend(); ++it) {
            if (!in) {
                goto rollback;
            }
            auto type = *it;
            switch (type) {
            case NodeType::Quote:
            case NodeType::Callout:
                if (in.peek() != '>') {
                    goto rollback;
                }
                std::ignore = in.get();
                if (in.peek() == ' ') {
                    std::ignore = in.get();
                }
                break;
            case NodeType::UnorderedList:
                if (includeBullets && in.peek() == '*') {
                    std::ignore = in.get();
                    if (in.peek() == ' ') {
                        std::ignore = in.get();
                    }
                } else if (
                    !in.readsome(buff.data(), buff.size())
                    || std::string_view{buff} != "  "
                ) {
                    goto rollback;
                }
                break;
            case NodeType::OrderedList:
                int n;
                if (includeBullets && in >> n) {
                    if (in.peek() != '.') {
                        goto rollback;
                    }
                    in.get();
                    if (in.peek() == ' ') {
                        in.get();
                    }
                } else if (
                    !in.readsome(buff.data(), buff.size())
                    || std::string_view{buff} != "  "
                ) {
                    goto rollback;
                }
                break;
            case NodeType::FootnoteDef:
                char ch;
                if (includeBullets && in.peek() == '[') {
                    std::ignore = in.get();
                    if (in.peek() != '^') {
                        goto rollback;
                    }

                    while (in && in >> std::noskipws >> ch) {
                        if (ch == ']') {
                            if (in.peek() != ':') {
                                goto rollback;
                            }
                            std::ignore = in.get();
                            if (in.peek() == ' ') {
                                std::ignore = in.get();
                            }
                            break;
                        }
                    }
                } else if (
                    !in.readsome(buff.data(), buff.size())
                    || std::string_view{buff} != "  "
                ) {
                    goto rollback;
                }
                break;
            default:
                throw std::runtime_error(
                    "prepareStream for loop got type that should've been filtered out by while loop"
                );
            }
        }
    }

    return true;
 rollback:
    in.seekg(start);
    in.clear();
    return false;
}

void Markdown::parseCodeBlockContent(
    std::stringstream& in,
    CodeNode* out,
    DocumentContext& context
) {
    char ch;
    std::stringstream content;
    prepareStream(in, out);
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

void Markdown::parseHeader(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
) {
    // TODO: This is not an elegant way to do this
    char ch;
    std::stringstream content;

    size_t level = 0;
    bool computingLevel = true;
    while (in >> std::noskipws >> ch) {
        if (ch == '#') {
            ++level;
            if (in.peek() != '#') {
                if (in.peek() == ' ') {
                    std::ignore = in.get();
                }
                break;
            }
        }
    }
    level = std::min((size_t) 6, level);

    auto node = new HeaderNode(
        level
    );
    out->addChild(
        node
    );
    parseParagraphContent(in, node, context);
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
    }

    auto node = new CodeNode(
        boundry,
        false,
        lang.str()
    );
    out->addChild(node);

    parseCodeBlockContent(in, node, context);
}

void Markdown::parseQuote(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
) {
    auto node = new BlockquoteNode();
    out->addChild(node);

    while (in) {
        if (!nextMajorMode(in, node, context)) {
            break;
        }
    }
}

void Markdown::parseCallout(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
) {
    std::stringstream calloutType;
    char ch;

    if (!prepareStream(in, out)) {
        throw TracedException(
            "prepareStream failed on leading callout line"
        );
    }
    // The prepareStream above does not consume the current quote, so we need to deal with that too
    if (in.peek() == '>') {
        std::ignore = in.get();
    }
    while (in.peek() == ' ') {
        std::ignore = in.get();
    }

    if (in.peek() != '[') {
        [[unlikely]]
        throw TracedException("Failed to parse callout");
    }
    std::ignore = in.get();
    if (in.peek() != '!') {
        [[unlikely]]
        throw TracedException("Failed to parse callout");
    }
    std::ignore = in.get();
    while (in >> std::noskipws >> ch) {
        if (ch == '\n') {
            throw SyntaxError(
                "Callout terminated with newline",
                in.tellg()
            );
        } else if (ch == ']') {
            if (in.peek() != '\n' && in.peek() >= 0) {
                throw SyntaxError(
                    "Callout contains illegal trailing text",
                    in.tellg()
                );
            }
            std::ignore = in.get();
            break;
        }
        calloutType << (char) std::tolower(ch);
    }
    auto node = new CalloutNode(
        calloutType.str(), in.tellg()
    );
    out->addChild(node);

    while (in) {
        if (!nextMajorMode(in, node, context)) {
            break;
        }
    }
}

void Markdown::parseUnorderedList(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
) {
    auto node = new UnorderedListNode();
    out->addChild(node);

    while(in && resolveMajorMode(in, out) == NodeType::UnorderedList) {
        auto entry = new UnorderedListEntryNode();
        node->addChild(entry);
        bool bulletBoundries = true;
        while (in && nextMajorMode(in, entry, context, bulletBoundries)) {
            bulletBoundries = false;
        }
    }
}

void Markdown::parseAnchorDef(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
) {
    if (in.peek() != '[') {
        throw std::runtime_error("Bad major mode parsing");
    }
    std::ignore = in.get();
    std::stringstream refName;
    std::stringstream url;

    char ch;
    while (in && in >> std::noskipws >> ch) {
        if (ch == ']') {
            if (in.peek() != ':') {
                throw std::runtime_error("Bad major mode parsing: ] not followed by :");
            }
            std::ignore = in.get();
            while (in.peek() == ' ') {
                std::ignore = in.get();
            }
            break;
        }

        refName << ch;
    }
    while (in && in >> std::noskipws >> ch) {
        if (ch == '\n') {
            break;
        }
        urlEncode(
            ch,
            url,
            false
        );
    }

    if (refName.tellp() == 0 || url.tellp() == 0) {
        throw std::runtime_error("Cannot have urlref definition with no refname and/or url");
    }

    context.externalLinkMap[refName.str()] = url.str();
}

void Markdown::parseFootnoteDef(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
) {
    size_t start = in.tellg();
    if (in.peek() != '[') {
        throw std::runtime_error("Bad major mode parsing");
    }
    std::ignore = in.get();
    if (in.peek() != '^') {
        throw std::runtime_error("Bad major mode parsing");
    }
    std::ignore = in.get();

    std::stringstream refName;
    auto root = new FootnoteDefNode;

    char ch;
    while (in && in >> std::noskipws >> ch) {
        if (ch == ']') {
            if (in.peek() != ':') {
                throw std::runtime_error("Bad major mode parsing: ] not followed by :");
            }
            std::ignore = in.get();
            while (in.peek() == ' ') {
                std::ignore = in.get();
            }
            break;
        } else if (ch == '\n') {
            throw SyntaxError("Newline in footnote def", in.tellg());
        }

        refName << ch;
    }

    // TODO: I don't think rewinding here is necessary, but I included it for troubleshooting what became the fix in
    // 1d30477, and now I don't feel like removing it again.
    // All this does is make the prepareStream call at the start of parseParagraph to actually work. More research
    // required.
    in.seekg(start);
    in.clear();

    while (in) {
        while (in.peek() == '\n') {
            std::ignore = in.get();
        }
        if (!nextMajorMode(in, root, context)) {
            break;
        }
    }
    context.footnotes[refName.str()] = root;
}

void Markdown::parseOrderedList(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
) {
    auto node = new OrderedListNode();
    out->addChild(node);

    while(in && resolveMajorMode(in, out) == NodeType::OrderedList) {
        auto entry = new OrderedListEntryNode();
        node->addChild(entry);
        bool bulletBoundries = true;
        while (in && nextMajorMode(in, entry, context, bulletBoundries)) {
            bulletBoundries = false;
        }
    }
}

bool Markdown::nextMajorMode(
    std::stringstream& in,
    DOMTree* tree,
    DocumentContext& context,
    bool bulletBoundries
) {
    switch (resolveMajorMode(in, tree, bulletBoundries)) {
    case NodeType::Paragraph: {
        parseParagraph(in, tree, context);
    } break;
    case NodeType::CodeBlock: {
        parseCodeBlock(in, tree, context);
    } break;
    case NodeType::Header: {
        parseHeader(in, tree, context);
    } break;
    case NodeType::Quote: {
        parseQuote(in, tree, context);
    } break;
    case NodeType::Callout: {
        parseCallout(in, tree, context);
    } break;
    case NodeType::UnorderedList: {
        parseUnorderedList(in, tree, context);
    } break;
    case NodeType::OrderedList: {
        parseOrderedList(in, tree, context);
    } break;
    case NodeType::AnchorDef: {
        parseAnchorDef(in, tree, context);
    } break;
    case NodeType::FootnoteDef: {
        parseFootnoteDef(in, tree, context);
    } break;
    case NodeType::BlockEnd:
        return false;
    default:
        throw std::runtime_error(
            "Unresolved or unimplemented major mode"
        );
    }
    return true;
}

std::string Markdown::parse(
    std::stringstream& in,
    const LinkTranslator& linkTranslator
) {
    DOMTree rootTree(NodeType::DocumentRoot);
    DocumentContext context(linkTranslator);

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

        nextMajorMode(in, &rootTree, context, true);
    }

    return stringifyTree(rootTree, context);
}

}
