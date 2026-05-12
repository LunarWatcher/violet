#include "violet/parsing/Markdown.hpp"

#include "violet/parsing/markdown/MetaNodes.hpp"

namespace violet {

void Markdown::parseTemplate(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext& context
) {
    prepareStream(in, out, true);
    std::stringstream ss;
    ss << "\n";
    char ch;
    while (in >> std::noskipws >> ch) {
        ss << ch;
        if (ch == '\n') {
            break;
        }
    }

    auto node = new TemplateNode();
    out->addChild(
        node
    );

    node->addChild(
        new ContentNode(std::move(ss.str()))
    );
}

}
