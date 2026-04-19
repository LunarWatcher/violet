#include "Markdown.hpp"
#include "violet/exceptions/SyntaxError.hpp"
#include "violet/parsing/Escaping.hpp"
#include "violet/parsing/LinkTranslate.hpp"
#include <iomanip>
#include <ios>
#include <iostream>
#include <stdexcept>

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
    } else if (in.peek() == '\n') { // Consume and ignore empty lines not consumed by previous modes
        std::ignore = in.get();
        // TODO: if this causes stackoverflows, add another meta-mode so the iterative approach can continue to be used
        return resolveMajorMode(in, tree, true);
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
            }
        }

        mode = NodeType::Paragraph;
    }
 done:
    // Reset the stream so the parsers can handle it properly
    in.seekg(offset);
    in.clear();
    return mode;
}

void commitContentNode(
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
    while (curr->parent != nullptr) {
        switch (curr->type) {
        case NodeType::Quote:
        case NodeType::UnorderedList:
        case NodeType::OrderedList:
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
    CodeNode* out
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

void Markdown::parseParagraphContent(
    std::stringstream& in,
    DOMTree* out
) {
    char ch;

    std::stringstream content;

    // Avoids some edge-cases where a single newline _does_ mark the end of a paragraph.
    bool hasDanglingNewline = false;

    prepareStream(in, out, true);

    while (in >> std::noskipws >> ch) {
        if (hasDanglingNewline) {
            content << " ";
            hasDanglingNewline = false;
        }
        if (ch == '*') {
            auto isBold = in.peek() == '*';
            if (isBold) {
                std::ignore = in.get();
            }
            bool isBoldItalic = isBold && in.peek() == '*';
            if (isBoldItalic) {
                std::ignore = in.get();
            }

            // TODO: this will handle splitting poorly, specifically of
            //      ***bold italic* just bold**
            //      ***bold italic** just italic*
            // since these need to convert to
            //      <b><i>bold italic</i> just bold</b>
            //      <i><b>bold italic</b> just italic</i>
            // and there's no way to resolve this at opening *** resolution time
            // It'll likely require using BoldItalicNode as an intermediate step.
            // Just not entirely sure how to resolve the rendering of the node right now, so leaving that as a future
            // problem.
            DOMTree* node;
            if (isBoldItalic) {
                if (out->type == NodeType::BoldItalic) {
                    break;
                } else {
                    node = new BoldItalicNode();
                }
            } else if (isBold) {
                if (out->type == NodeType::Bold) {
                    break;
                } else {
                    auto* curr = out;
                    bool hasItalic = false;
                    while (!hasItalic && curr->parent != nullptr) {
                        hasItalic = curr->type == NodeType::Italic;
                        curr = curr->parent;
                    }
                    node = new BoldNode(hasItalic);
                }
            } else {
                if (out->type == NodeType::Italic) {
                    break;
                } else {
                    auto* curr = out;
                    bool hasBold = false;
                    while (!hasBold && curr->parent != nullptr) {
                        hasBold = curr->type == NodeType::Bold;
                        curr = curr->parent;
                    }
                    node = new ItalicNode(hasBold);
                }
            }

            commitContentNode(content, out);
            out->addChild(node);
            parseParagraphContent(in, node);
        } else if (ch == '`') {
            commitContentNode(content, out);
            size_t boundry = 1;
            while (in.peek() == '`') {
                std::ignore = in.get();
                ++boundry;
            }

            auto node = new CodeNode(boundry, true);
            out->addChild(node);

            parseCodeBlockContent(in, node);
        } else if (ch == '[') {
            commitContentNode(content, out);
            if (out->type == NodeType::Anchor
                || out->type == NodeType::Footnote) {
                throw SyntaxError(
                    "Unescaped [ inside anchor",
                    in.tellg()
                );
            }
            if (in.peek() == '^') {
                // TODO
            } else {
                auto urlNode = new URLNode();
                out->addChild(urlNode);
                parseParagraphContent(in, urlNode);
            }
        } else if (out->type == NodeType::Anchor && ch == ']') {
            auto node = static_cast<URLNode*>(out);
            if (in.peek() == '[' || in.peek() == '(') {
                // [text][ref] or [text](url)
                auto open = in.get();
                auto close = open == '[' ? ']' : ')';

                std::stringstream urlOrRef;
                while (in >> std::noskipws >> ch) {
                    if (ch == close) {
                        break;
                    }
                    if (open == '(') {
                        urlEncode(
                            ch,
                            urlOrRef,
                            false
                        );
                    } else {
                        // references have to be unmodified
                        urlOrRef << ch;
                    }
                }
                node->urlType = open == '(' ? URLNode::Type::Standard : URLNode::Type::Reference;
                node->urlOrRef = open == '(' ? stringifyAndTranslateUrl(urlOrRef) : urlOrRef.str();
                
            } else if (in.peek() == ':') {
                // TODO: how should this work with the tree? These nodes should technically disappear from the tree
                // itself, since they'd be converted to a context object.
            } else {
                node->urlType = URLNode::Type::Reference;
                node->urlOrRef = content.str();
            }

            commitContentNode(content, out);
            break;
        } else if (ch == '\n') {
            if (out->type == NodeType::Anchor) {
                throw SyntaxError(
                    "Anchor cannot contain newline",
                    in.tellg()
                );
            } else if (out->type != NodeType::Paragraph) {
                // Non-paragraph invoking types are likely just using this function for core markdown parsing
                break;
            } else if (in.peek() == '\n') {
                // End of paragraph
                std::ignore = in.get();
                break;
            }
            hasDanglingNewline = true;

            auto mode = resolveMajorMode(
                in,
                out,
                false
            );
            // Re-check the mode to account for edge-cases like a single line of code being immediately followed by a
            // list, quote, or code block
            if (mode != NodeType::Paragraph) {
                break;
            }
            if (!prepareStream(
                in,
                out,
                false
            )) {
                throw std::runtime_error("Unexpected indent");
            }
        } else {
            content << ch;
        }
    }

    commitContentNode(content, out);
}

void Markdown::parseHeader(
    std::stringstream& in,
    DOMTree* out
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
    parseParagraphContent(in, node);
}

void Markdown::parseParagraph(
    std::stringstream& in,
    DOMTree* tree
) {
    DOMTree* root = new DOMTree(NodeType::Paragraph);
    tree->addChild(root);
    parseParagraphContent(in, root);

}

void Markdown::parseCodeBlock(
    std::stringstream& in,
    DOMTree* out
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

    parseCodeBlockContent(in, node);
}

void Markdown::parseQuote(
    std::stringstream& in,
    DOMTree* out
) {
    auto node = new BlockquoteNode();
    out->addChild(node);

    while (in) {
        if (!nextMajorMode(in, node)) {
            break;
        }
    }
}

void Markdown::parseUnorderedList(
    std::stringstream& in,
    DOMTree* out
) {
    auto node = new UnorderedListNode();
    out->addChild(node);

    while(in && resolveMajorMode(in, out) == NodeType::UnorderedList) {
        auto entry = new UnorderedListEntryNode();
        node->addChild(entry);
        bool bulletBoundries = true;
        while (in && nextMajorMode(in, entry, bulletBoundries)) {
            bulletBoundries = false;
        }
    }
}

void Markdown::parseOrderedList(
    std::stringstream& in,
    DOMTree* out
) {
    auto node = new OrderedListNode();
    out->addChild(node);

    while(in && resolveMajorMode(in, out) == NodeType::OrderedList) {
        auto entry = new OrderedListEntryNode();
        node->addChild(entry);
        bool bulletBoundries = true;
        while (in && nextMajorMode(in, entry, bulletBoundries)) {
            bulletBoundries = false;
        }
    }
}

bool Markdown::nextMajorMode(
    std::stringstream& in,
    DOMTree* tree,
    bool bulletBoundries
) {
    switch (resolveMajorMode(in, tree, bulletBoundries)) {
    case NodeType::Paragraph: {
        parseParagraph(in, tree);
    } break;
    case NodeType::CodeBlock: {
        parseCodeBlock(in, tree);
    } break;
    case NodeType::Header: {
        parseHeader(in, tree);
    } break;
    case NodeType::Quote: {
        parseQuote(in, tree);
    } break;
    case NodeType::UnorderedList: {
        parseUnorderedList(in, tree);
    } break;
    case NodeType::OrderedList: {
        parseOrderedList(in, tree);
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

std::string Markdown::parse(std::stringstream& in) {
    DOMTree rootTree(NodeType::DocumentRoot);

    while (in) {
        nextMajorMode(in, &rootTree);
    }

    return stringifyTree(rootTree);
}

void stringifyTreeImpl(const Markdown::DOMTree* tree, std::stringstream& ss) {

    switch (tree->type) {
    case Markdown::NodeType::Content: {
        ss << static_cast<const Markdown::ContentNode*>(tree)->paragraph;
    } break;
    case Markdown::NodeType::Paragraph: {
        ss << "<p>";
    } break;
    case Markdown::NodeType::InlineCode: {
        ss << "<code>";
    } break;
    case Markdown::NodeType::CodeBlock: {
        ss << "<pre><code";
        auto* node = static_cast<const Markdown::CodeNode*>(tree);
        if (!node->language.empty()) {
            ss << " class=\"language-" << node->language << "\"";
        }
        ss << ">";
    } break;
    case Markdown::NodeType::Header: {
        ss << "<h" << static_cast<const Markdown::HeaderNode*>(tree)->level << ">";
    } break;
    case Markdown::NodeType::Quote: {
        ss << "<blockquote>";
    } break;
    case Markdown::NodeType::Anchor: {
        // TODO: actually parse refs
        ss << "<a href=\"" << static_cast<const Markdown::URLNode*>(tree)->urlOrRef << "\">";
        break;
    }
    case Markdown::NodeType::Bold: {
        ss << "<strong>";
    } break;
    case Markdown::NodeType::Italic: {
        ss << "<em>";
    } break;
    case Markdown::NodeType::BoldItalic: {
        ss << "<strong><em>";
    } break;
    case Markdown::NodeType::UnorderedList:
        ss << "<ul>";
        break;
    case Markdown::NodeType::OrderedList:
        ss << "<ol>";
        break;
    case Markdown::NodeType::OrderedListEntry:
    case Markdown::NodeType::UnorderedListEntry:
        ss << "<li>";
        break;

    default:
      break;
    }

    for (size_t i = 0; i < tree->children.size(); ++i) {
        auto& node = tree->children.at(i);
        // if (i > 0 && tree->type == Markdown::NodeType::Paragraph) {
        //     ss << " ";
        // }

        stringifyTreeImpl(node, ss);
    }

    switch (tree->type) {
    case Markdown::NodeType::Paragraph: {
        ss << "</p>";
    } break;
    case Markdown::NodeType::InlineCode: {
        ss << "</code>";
    } break;
    case Markdown::NodeType::CodeBlock: {
        ss << "</code></pre>";
    } break;
    case Markdown::NodeType::Header: {
        ss << "</h" << static_cast<const Markdown::HeaderNode*>(tree)->level << ">";
    } break;
    case Markdown::NodeType::Quote: {
        ss << "</blockquote>";
    } break;
    case Markdown::NodeType::Anchor: {
        ss << "</a>";
    } break;
    case Markdown::NodeType::Bold: {
        ss << "</strong>";
    } break;
    case Markdown::NodeType::Italic: {
        ss << "</em>";
    } break;
    case Markdown::NodeType::BoldItalic: {
        ss << "</em></strong>";
    } break;
    case Markdown::NodeType::UnorderedList:
        ss << "</ul>";
        break;
    case Markdown::NodeType::OrderedList:
        ss << "</ol>";
        break;
    case Markdown::NodeType::OrderedListEntry:
    case Markdown::NodeType::UnorderedListEntry:
        ss << "</li>";
        break;
    default:
      break;
    }
}

std::string Markdown::stringifyTree(const DOMTree& tree) {
    std::stringstream ss;
    stringifyTreeImpl(&tree, ss);
    return ss.str();
}

}
