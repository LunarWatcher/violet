#include "violet/parsing/Markdown.hpp"

#include "violet/parsing/markdown/ContextConsumingNodes.hpp"
#include "violet/parsing/markdown/DOMTree.hpp"
#include "violet/parsing/markdown/DocumentContext.hpp"
#include "violet/parsing/markdown/ElementaryNodes.hpp"
#include <iostream>
#include <sstream>

namespace violet {

static void stringifyTreeImpl(
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
        auto* headerNode = static_cast<const Markdown::HeaderNode*>(tree);
        ss << "<h" << headerNode->level
           << " id=\"" << headerNode->id << "\""
           << ">";
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
    case Markdown::NodeType::Image: {
        auto imgNode = static_cast<const Markdown::ImageNode*>(tree);
        ss << "<img"
           << " src=\"" << imgNode->getUrl(context) << "\""
           << " alt=\"" << imgNode->alt << "\""
           << " />";
    } break;
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
                std::cout << "Failed to match " << node->ref << ". Found:" << std::endl;
                for (auto& [k, _] : context.footnotes) {
                    std::cout << "Footnote ref: " << k << std::endl;
                }
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
    const DOMTree* tree,
    DocumentContext& context
) {
    std::stringstream ss;
    stringifyTreeImpl(
        tree,
        ss,
        context
    );

    if (
        tree->type == NodeType::DocumentRoot &&
        !context.usedFootnotes.empty()
    ) {
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

std::string Markdown::serializeHeaders(
    DOMTree& root,
    DocumentContext& ctx
) {
    size_t baseLevel = 90;

    std::stringstream ss;
    // This is not optimal, but here we filter to find the actual root level. If the file only contains ## and down (up?
    // whatever we count more #'s as), there's no point in treating h1 as missing. It'll likely be set by the page, and
    // we don't necessarily want to include it.
    // TODO: add an option to explicitly exclude levels (instead?) - would that be a site setting?
    // Could probably also defer to JS or something on the page
    for (size_t i = 0; i < root.children.size(); ++i) {
        auto* node = root.children.at(i);
        if (node->type == NodeType::Header) {
            baseLevel = std::min(baseLevel, static_cast<HeaderNode*>(node)->level - 1);
        }
    }

    // No headers
    if (baseLevel == 90) {
        [[unlikely]]
        return "";
    }

    auto lastLevel = 0;

    ss << "<div id=\"violet-toc\">";

    for (size_t i = 0; i < root.children.size(); ++i) {
        auto* node = root.children.at(i);

        if (node->type == NodeType::Header) {
            auto* headerNode = static_cast<HeaderNode*>(node);
            auto correctedLevel = headerNode->level - baseLevel;
            if (correctedLevel > lastLevel) {
                for (size_t i = 0; i < correctedLevel - lastLevel; ++i) {
                    ss << "<ol><li>";
                }
            } else if (correctedLevel == lastLevel) {
                ss << "</li><li>";
            } else {
                for (size_t i = 0; i < lastLevel - correctedLevel; ++i) {
                    ss << "</li></ol>";
                }
                ss << "</li><li>";
            }

            URLNode virtualURLNode;
            virtualURLNode.urlType = URLNode::Type::Standard;
            virtualURLNode.urlOrRef = std::format("#{}", headerNode->id);
            virtualURLNode.isVirtual = true;
            for (auto& contentNodes : node->children) {
                virtualURLNode.addChild(contentNodes);
            }

            stringifyTreeImpl(&virtualURLNode, ss, ctx);

            lastLevel = correctedLevel;
        }
    }
    for (size_t i = 0; i < lastLevel; ++i) {
        ss << "</li></ol>";
    }
    ss << "</div>";
    return ss.str();
}

}
