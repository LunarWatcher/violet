#pragma once

#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>

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
    UnorderedList = 6,
    OrderedList = 7,
    Header = 8,
    // Span styles
    Bold = 9,
    Italic = 10,
    BoldItalic = 11,
    InlineCode = 12,
    Anchor = 13,
    Footnote = 14,

    UnorderedListEntry = 15,
    OrderedListEntry = 16,
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

struct ContentNode : public DOMTree {
    const std::string paragraph;

    ContentNode(std::string&& content) : DOMTree(NodeType::Content), paragraph(std::move(content)) {}
};

struct HeaderNode : public DOMTree {
    const size_t level;

    HeaderNode(
        size_t level
    ) : DOMTree(NodeType::Header), level(level) {}
};

struct BlockquoteNode : public DOMTree {
    BlockquoteNode() : DOMTree(NodeType::Quote) {}
};

struct CodeNode : public DOMTree {
    const size_t boundrySize;
    const std::string language;

    CodeNode(
        size_t boundrySize,
        bool isInline,
        std::string&& language = ""
    ) : DOMTree(isInline ? NodeType::InlineCode : NodeType::CodeBlock),
        boundrySize(boundrySize),
        language(std::move(language))
    {}
};

struct BoldNode : public DOMTree {
    /**
     * Whether or not the node is within an ItalicNode.
     * Used to optimise parsing
     */
    const bool contained;
    BoldNode(
        bool contained
    ) : DOMTree(NodeType::Bold), contained(contained) {}
};
struct ItalicNode : public DOMTree {
    /**
     * Whether or not the node is within a BoldNode
     * Used to optimise parsing
     */
    const bool contained;
    ItalicNode(
        bool contained
    ) : DOMTree(NodeType::Italic), contained(contained) {}
};
struct BoldItalicNode : public DOMTree {
    bool isBoldLead = true;
    BoldItalicNode() : DOMTree(NodeType::BoldItalic) {}
};

struct UnorderedListNode : public DOMTree {
    UnorderedListNode() : DOMTree(NodeType::UnorderedList) {}
};
struct UnorderedListEntryNode : public DOMTree {
    UnorderedListEntryNode() : DOMTree(NodeType::UnorderedListEntry) {}
};

struct OrderedListNode : public DOMTree {
    OrderedListNode() : DOMTree(NodeType::OrderedList) {}
};
struct OrderedListEntryNode : public DOMTree {
    OrderedListEntryNode() : DOMTree(NodeType::OrderedListEntry) {}
};

struct URLNode : public DOMTree {
    std::string urlOrRef;
    enum class Type {
        // [text](url)
        Standard,
        // [text][urlref] or [urlref]
        Reference,
    };

    Type urlType;

    URLNode() : DOMTree(NodeType::Anchor) {}
};

struct FootnoteNode : public DOMTree {
    FootnoteNode() : DOMTree(NodeType::Footnote) {}
};

struct DocumentContext {
    std::unordered_map<std::string, std::string> externalLinkMap;
    std::unordered_map<std::string, FootnoteNode*> footnotes;

    ~DocumentContext() {
        for (auto& [_, node] : footnotes) {
            delete node;
        }
    }
};

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

extern std::string parse(std::stringstream& in);

}
