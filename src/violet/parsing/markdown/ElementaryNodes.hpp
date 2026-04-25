#pragma once

#include "DocumentContext.hpp"
#include "DOMTree.hpp"
#include "violet/exceptions/SyntaxError.hpp"

#include <string>
#include <unordered_set>

namespace violet::Markdown {

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

struct CalloutNode : public DOMTree {
    // TODO: not sure if I want to be this strict, or if I want to allow arbitrary types
    static const inline std::unordered_set<std::string> legalCalloutTypes = {
        "note", "info", "tip", "important", "warning", "caution"
    };

    const std::string type;
    CalloutNode(
        std::string&& type,
        size_t errorIdx
    )
        : DOMTree(NodeType::Callout),
          type(std::move(type))
    {
        if (auto it = legalCalloutTypes.find(this->type); it == legalCalloutTypes.end()) {
            throw SyntaxError(
                "Invalid callout type. Must be one of: "
                "note, info, tip, important, warning, caution, but found "
                + this->type,
                errorIdx
            );
        }
    }
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

struct FootnoteNode : public DOMTree {
    const std::string ref;
    FootnoteNode(std::string&& ref)
        : DOMTree(NodeType::Footnote),
          ref(std::move(ref))
    {}
};

}
