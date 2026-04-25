#pragma once

#include "DOMTree.hpp"

namespace violet::Markdown {


struct FootnoteDefNode : public DOMTree {
    FootnoteDefNode() : DOMTree(NodeType::FootnoteDef) {}
};

}
