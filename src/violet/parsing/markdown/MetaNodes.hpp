#pragma once

#include "violet/parsing/markdown/DOMTree.hpp"

namespace violet::Markdown {

struct TemplateNode : public DOMTree {
    TemplateNode()
        : DOMTree(NodeType::Template)
    {}
};

}
