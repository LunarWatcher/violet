#include "violet/parsing/Markdown.hpp"

#include "violet/parsing/markdown/MetaNodes.hpp"
#include "violet/util/LockStreamPos.hpp"

namespace violet {

void Markdown::parseTemplate(
    std::stringstream& in,
    DOMTree* out,
    DocumentContext&
) {
    thread_local std::array<char, 2> buff;
    prepareStream(in, out, true);
    std::stringstream ss;
    ss << "\n";
    char ch;
    while (in >> std::noskipws >> ch) {
        ss << ch;
        if (ch == '\n') {
            LockStreamPos lock(in);

            // If we have a newline, we conditionally break. We first prepare the stream to see if we have broken out of
            // the parent. This avoids contamination across bullets
            if (!prepareStream(in, out, false)) {
                break;
            }

            // If the stream prep worked, we check if the next item is a space or a tab (tab checks are not done broadly
            // enough at this time, but it really should be checked more).
            // If it isn't, and it isn't any of the template or HTML markers, we consider the template complete
            if (
                in.peek() != ' '
                && in.peek() != '\t'
                && in.peek() != '<'
                && in.peek() != '{'
            ) {
                break;
            }

            lock.commit();
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
