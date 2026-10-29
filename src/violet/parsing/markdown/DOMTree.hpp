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
    Paragraph = 10,
    CodeBlock = 11,
    Quote = 12,
    Callout = 13,
    UnorderedList = 14,
    OrderedList = 15,
    Header = 16,
    AnchorDef = 17,
    FootnoteDef = 18,

    // Span styles
    Bold = 100,
    Italic = 101,
    BoldItalic = 102,
    InlineCode = 103,
    Anchor = 104,
    Footnote = 105,
    Strike = 106,
    Del = 107,

    UnorderedListEntry = 200,
    UnorderedListChecklistEntry = 201,
    OrderedListEntry = 202,

    Image = 300,
    HorizontalRule = 301,
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
