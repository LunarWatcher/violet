#include "violet/parsing/Escaping.hpp"
#include "violet/parsing/LinkTranslate.hpp"
#include "violet/parsing/Markdown.hpp"
#include "violet/parsing/markdown/ContextConsumingNodes.hpp"
#include <iostream>

namespace violet {

void Markdown::parseParagraphContent(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
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
            parseParagraphContent(in, node, context);
        } else if (ch == '`') {
            commitContentNode(content, out);
            size_t boundry = 1;
            while (in.peek() == '`') {
                std::ignore = in.get();
                ++boundry;
            }

            auto node = new CodeNode(boundry, true);
            out->addChild(node);

            parseCodeBlockContent(in, node, context);
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
                std::ignore = in.get();
                std::stringstream ref;
                while (in >> std::noskipws >> ch) {
                    if (ch == '\n') {
                        throw SyntaxError(
                            "Unterminated footnote reference",
                            in.tellg()
                        );
                    }
                    if (ch == ']') {
                        break;
                    }
                    ref << ch;
                }

                if (in.tellp() == 0) {
                    throw SyntaxError(
                        "Empty footnote ref",
                        in.tellg()
                    );
                }
                auto footnoteRef = new FootnoteNode(ref.str());
                out->addChild(footnoteRef);
            
            } else if (in.peek() == '!') {
                content << "[";
                std::cerr << "Paragraph consumed callout marker at " << in.tellg() << std::endl;
            } else {
                auto urlNode = new URLNode;
                out->addChild(urlNode);
                parseParagraphContent(in, urlNode, context);
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
            }
            if (in.peek() == '\n') {
                // End of paragraph
                // If we're in a quote or callout, don't consume the newline. If the newline is consumed, the
                // blockquotes cannot be separated by a single line, or they'll join together
                // We can't unconditionally not consume if not under the document root, as this would break the
                // intentional support for this in lists, i.e.
                // * List
                //
                //   Continuation paragraph
                if (
                    out->type == NodeType::Paragraph
                    && out->parent != nullptr
                    && out->parent->type != NodeType::Quote
                    && out->parent->type != NodeType::Callout
                ) {
                    std::ignore = in.get();
                }
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

            // equivalent to the last \n check, but in the context of the block. We have a block continuation that is
            // also a paragraph end
            if (in.peek() == '\n') {
                std::ignore = in.get();
                break;
            }
        } else {
            content << ch;
        }
    }

    commitContentNode(content, out);
}

void Markdown::parseParagraph(
    std::stringstream& in,
    DOMTree* tree,
    DocumentContext& context
) {
    DOMTree* root = new DOMTree(NodeType::Paragraph);
    tree->addChild(root);
    parseParagraphContent(in, root, context);

}


}
