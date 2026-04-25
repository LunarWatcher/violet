#pragma once

#include <vector>
namespace violet::Markdown {

enum class NodeType {
    // Special: text content only
    Content = 0,
    // Special: the node that contains the document root
    DocumentRoot = 1,
    // Special: The previous block has ended
    BlockEnd = 2,

    // Root nodes
    Paragraph = 3,
    CodeBlock = 4,
    Quote = 5,
    Callout = 6,
    UnorderedList = 7,
    OrderedList = 8,
    Header = 9,
    AnchorDef = 10,
    FootnoteDef = 11,
    // Span styles
    Bold = 12,
    Italic = 13,
    BoldItalic = 14,
    InlineCode = 15,
    Anchor = 16,
    Footnote = 17,

    UnorderedListEntry = 18,
    OrderedListEntry = 19,
};

struct DOMTree {
    DOMTree* parent = nullptr;

    std::vector<DOMTree*> children;
    NodeType type;

    DOMTree(NodeType type) : type(type) {}

    virtual ~DOMTree() {
        for (auto child : children) {
            delete child;
        }
    }

    void addChild(DOMTree* node) {
        node->parent = this;
        children.push_back(node);
    }
};

}
