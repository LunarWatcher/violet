#pragma once

#include "violet/exceptions/SyntaxError.hpp"
#include "violet/parsing/markdown/DOMTree.hpp"
#include "violet/parsing/markdown/DocumentContext.hpp"
#include <string>
#include <utility>

namespace violet::Markdown {

struct LinkableNode : public DOMTree {
    std::string urlOrRef;
    enum class Type {
        // [text](url)
        Standard,
        // [text][urlref] or [urlref]
        Reference,
    };

    Type urlType;

    LinkableNode(NodeType type) : DOMTree(type) {}

    virtual ~LinkableNode() = default;

    std::string getUrl(DocumentContext& context) const {
        switch (urlType) {
        case Type::Standard:
            return urlOrRef;
        case Type::Reference: {
            auto it = context.externalLinkMap.find(urlOrRef);
            if (it == context.externalLinkMap.end()) {
                throw SyntaxError(
                    "url reference " + urlOrRef + " is used in text, but not defined.",
                    -1
                );
            }
            return it->second;
        }
        }
        std::unreachable();
    }
};

struct URLNode : public LinkableNode {
    URLNode() : LinkableNode(NodeType::Anchor) {}
};

struct ImageNode : public LinkableNode {
    std::string alt;
    ImageNode() : LinkableNode(NodeType::Image) {}
};

}
