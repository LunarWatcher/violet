#include "violet/parsing/Markdown.hpp"

#include "violet/parsing/markdown/ContextConsumingNodes.hpp"
#include "violet/parsing/markdown/DOMTree.hpp"
#include "violet/parsing/markdown/DocumentContext.hpp"
#include <sstream>

namespace violet {

void stringifyTreeImpl(
    const Markdown::DOMTree* tree,
    std::stringstream& ss,
    Markdown::DocumentContext& context
) {

    switch (tree->type) {
    case Markdown::NodeType::Content: {
        ss << static_cast<const Markdown::ContentNode*>(tree)->paragraph;
    } break;
    case Markdown::NodeType::Paragraph: {
        // Avoid including empty paragraphs. Largely useful for avoiding weird shit with in-file inja markup
        if (tree->children.size() == 0) {
            return;
        }
        ss << "<p>";
    } break;
    case Markdown::NodeType::InlineCode: {
        ss << "<code>";
    } break;
    case Markdown::NodeType::CodeBlock: {
        ss << "<pre><code";
        auto* node = static_cast<const Markdown::CodeNode*>(tree);
        if (!node->language.empty()) {
            ss << " class=\"language-" << node->language << "\"";
        }
        ss << ">";
    } break;
    case Markdown::NodeType::Header: {
        ss << "<h" << static_cast<const Markdown::HeaderNode*>(tree)->level << ">";
    } break;
    case Markdown::NodeType::Quote: {
        ss << "<blockquote>";
    } break;
    case Markdown::NodeType::Callout: {
        auto type = std::string_view{
            static_cast<const Markdown::CalloutNode*>(tree)->type
        };
        ss << "<blockquote class=\"callout-" << type << "\">";
        ss << R"(<p class="callout-title">)" << (char) std::toupper(type[0])
           << type.substr(1) << "</p>";
    } break;
    case Markdown::NodeType::Anchor: {
        auto* node = static_cast<const Markdown::URLNode*>(tree);
        ss << "<a href=\"" << node->getUrl(context) << "\">";
        break;
    }
    case Markdown::NodeType::Bold: {
        ss << "<strong>";
    } break;
    case Markdown::NodeType::Italic: {
        ss << "<em>";
    } break;
    case Markdown::NodeType::BoldItalic: {
        ss << "<strong><em>";
    } break;
    case Markdown::NodeType::UnorderedList:
        ss << "<ul>";
        break;
    case Markdown::NodeType::OrderedList:
        ss << "<ol>";
        break;
    case Markdown::NodeType::OrderedListEntry:
    case Markdown::NodeType::UnorderedListEntry:
        ss << "<li>";
        break;
    default:
        break;
    }

    if (tree->type == Markdown::NodeType::Footnote) {
        auto node = static_cast<const Markdown::FootnoteNode*>(tree);
        size_t footnoteIdx = 0;
        for (size_t i = 0; i < context.usedFootnotes.size(); ++i) {
            if (
                context.usedFootnotes.at(i) == node->ref
            ) {
                footnoteIdx = i + 1;
                break;
            }
        }
        if (footnoteIdx == 0) {
            // Footnote has not been used yet
            if (auto it = context.footnotes.find(node->ref); it != context.footnotes.end()) {
                context.usedFootnotes.push_back(node->ref);
                footnoteIdx = context.usedFootnotes.size();
            } else {
                throw SyntaxError(
                    std::format(
                        "Footnote ref \"{}\" has not been defined",
                        node->ref
                    ),
                    0
                );
            }
        }

        size_t usageCount = ++context.footnoteUsageCounts[node->ref];

        ss << "<sup><a href=\"#fn-" << footnoteIdx << "\" "
           << std::format("id=\"fn-{}-r{}\"", footnoteIdx, usageCount)
           << ">[" << footnoteIdx << "]</a></sup>";
    } else {
        for (size_t i = 0; i < tree->children.size(); ++i) {
            auto& node = tree->children.at(i);
            // if (i > 0 && tree->type == Markdown::NodeType::Paragraph) {
            //     ss << " ";
            // }

            stringifyTreeImpl(node, ss, context);
        }
    }

    switch (tree->type) {
    case Markdown::NodeType::Paragraph: {
        ss << "</p>";
    } break;
    case Markdown::NodeType::InlineCode: {
        ss << "</code>";
    } break;
    case Markdown::NodeType::CodeBlock: {
        ss << "</code></pre>";
    } break;
    case Markdown::NodeType::Header: {
        ss << "</h" << static_cast<const Markdown::HeaderNode*>(tree)->level << ">";
    } break;
    case Markdown::NodeType::Quote:
    case Markdown::NodeType::Callout: {
        ss << "</blockquote>";
    } break;
    case Markdown::NodeType::Anchor: {
        ss << "</a>";
    } break;
    case Markdown::NodeType::Bold: {
        ss << "</strong>";
    } break;
    case Markdown::NodeType::Italic: {
        ss << "</em>";
    } break;
    case Markdown::NodeType::BoldItalic: {
        ss << "</em></strong>";
    } break;
    case Markdown::NodeType::UnorderedList:
        ss << "</ul>";
        break;
    case Markdown::NodeType::OrderedList:
        ss << "</ol>";
        break;
    case Markdown::NodeType::OrderedListEntry:
    case Markdown::NodeType::UnorderedListEntry:
        ss << "</li>";
        break;
    default:
      break;
    }
}

std::string Markdown::stringifyTree(
    const DOMTree& tree,
    DocumentContext& context
) {
    std::stringstream ss;
    stringifyTreeImpl(
        &tree,
        ss,
        context
    );

    if (!context.usedFootnotes.empty()) {
        ss << "<div id=\"violet-footnotes\">";
        ss << "<h2>Footnotes</h2>";
        ss << "<ol>";
        for (size_t i = 0; i < context.usedFootnotes.size(); ++i) {
            auto& footnote = context.usedFootnotes.at(i);
            ss << "<li id=\"fn-" << i + 1 << "\">";
            stringifyTreeImpl(
                context.footnotes.at(footnote),
                ss,
                context
            );
            ss << "<span class=\"footnote-backref\">Usages: ";

            for (size_t usageRef = 0; usageRef < context.footnoteUsageCounts.at(footnote); ++usageRef) {
                ss << std::format(
                    R"({2}<a href="#fn-{0}-r{1}">[{1}]</a>)",
                    i + 1,
                    usageRef + 1,
                    usageRef > 0 ? ", " : ""
                );
            }
            
            ss << "</span>";
            ss << "</li>";
        }
        ss << "</ol>";
        ss << "</div>";
    }

    return ss.str();
}

}
