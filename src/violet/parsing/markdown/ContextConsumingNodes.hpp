#pragma once

#include "violet/parsing/markdown/DOMTree.hpp"
#include "violet/parsing/markdown/DocumentContext.hpp"
#include <string>
#include <utility>

namespace violet::Markdown {

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

    std::string getUrl(DocumentContext& context) const {
        switch (urlType) {
        case Type::Standard:
            return urlOrRef;
        case Type::Reference:
            return context.externalLinkMap.at(urlOrRef);
        }
        std::unreachable();
    }
};


}
