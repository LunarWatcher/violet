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
    // Special: used for lines with {{ templates }}, as well as raw HTML. This MAY be a block that includes a whole
    // range.
    // Separate types are not used for {{ templates }} and <html> because it does not matter. Violet does not evaluate
    // these, so the parser does not need to care about the distinction between the two. Neither of them can be wrapped
    // in HTML, and that's all we need to care about.
    Template = 3,

    // Root nodes
    Paragraph = 4,
    CodeBlock = 5,
    Quote = 6,
    Callout = 7,
    UnorderedList = 8,
    OrderedList = 9,
    Header = 10,
    AnchorDef = 11,
    FootnoteDef = 12,
    // Span styles
    Bold = 13,
    Italic = 14,
    BoldItalic = 15,
    InlineCode = 16,
    Anchor = 17,
    Footnote = 18,

    UnorderedListEntry = 19,
    OrderedListEntry = 20,

    Image = 21,
    HorizontalRule = 22,

};

struct DOMTree {
    DOMTree* parent = nullptr;

    std::vector<DOMTree*> children;
    NodeType type;

    bool isVirtual = false;

    DOMTree(NodeType type) : type(type) {}

    virtual ~DOMTree() {
        if (!isVirtual) {
            for (auto child : children) {
                delete child;
            }
        }
    }

    virtual void addChild(DOMTree* node) {
        if (!isVirtual) {
            node->parent = this;
        }
        children.push_back(node);
    }
};

}
