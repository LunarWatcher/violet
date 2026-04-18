#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

using namespace violet::Markdown;

TEST_CASE("Test plain text parsing", "[markdown]") {
    std::stringstream ss;
    ss << "Trans rights are human rights\n\nThis should be a new paragraph";

    std::string result = violet::Markdown::parse(ss);

    REQUIRE(
        result == "<p>Trans rights are human rights</p><p>This should be a new paragraph</p>"
    );
}

TEST_CASE("Inline code", "[markdown]") {
    std::stringstream ss;
    ss << "`this is a code block`\n";

    SECTION("Major mode should be Paragraph") {
        DOMTree root(NodeType::DocumentRoot);
        REQUIRE(
            violet::Markdown::resolveMajorMode(ss, &root)
            ==
            violet::Markdown::NodeType::Paragraph
        );

        REQUIRE(ss.tellg() == 0);
    }

    SECTION("Parsing") {
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<p><code>this is a code block</code></p>"
        );
    }
}

TEST_CASE("Paragraph continuation", "[markdown]") {
    std::stringstream ss;
    std::string expected;

    SECTION("Code-code") {
        ss << "`test`\n`test`\n\n`test`";
        expected = "<p><code>test</code> <code>test</code></p><p><code>test</code></p>";
    }

    REQUIRE(
        violet::Markdown::parse(ss)
        ==
        expected
    );
}

TEST_CASE("Code blocks without language", "[markdown]") {
    std::stringstream ss;
    ss << R"(```
code
```)";

    DOMTree root(NodeType::DocumentRoot);
    SECTION("Mode") {
        REQUIRE(violet::Markdown::resolveMajorMode(ss, &root) == violet::Markdown::NodeType::CodeBlock);
    }

    SECTION("Parsing") {
        REQUIRE(violet::Markdown::parse(ss) == "<pre><code>code</code></pre>");
    }
}

TEST_CASE("Code blocks with language", "[markdown]") {
    std::stringstream ss;
    ss << R"(```cpp
code
```)";

    DOMTree root(NodeType::DocumentRoot);
    SECTION("Mode") {
        REQUIRE(violet::Markdown::resolveMajorMode(ss, &root) == violet::Markdown::NodeType::CodeBlock);
    }

    SECTION("Parsing") {
        REQUIRE(violet::Markdown::parse(ss) == "<pre><code class=\"language-cpp\">code</code></pre>");
    }
}

TEST_CASE("Permissiveness", "[markdown]") {

    SECTION("Paragraph -> code with no spacing") {
        std::stringstream ss;
        ss << R"(This is a line
```
this is a code block
```)";
        REQUIRE(violet::Markdown::parse(ss) == "<p>This is a line</p><pre><code>this is a code block</code></pre>");
    }
    SECTION("Header -> paragraph") {
        std::stringstream ss;
        ss << R"(# This is a header
This is a line)";
        REQUIRE(violet::Markdown::parse(ss) == "<h1>This is a header</h1><p>This is a line</p>");
    }
}

TEST_CASE("Context changing", "[markdown]") {
    SECTION("Quote rebasing") {
        std::stringstream ss;
        ss << "> Quote";

        DOMTree root(NodeType::DocumentRoot);
        auto node = new BlockquoteNode();
        root.addChild(node);

        REQUIRE(prepareStream(ss, node));

        REQUIRE(ss.tellg() == 2);
        std::string buff;
        std::getline(ss, buff);

        REQUIRE(buff == "Quote");
    }
}

TEST_CASE("Unordered lists", "[markdown]") {
    SECTION("Plain bullet lists") {
        std::stringstream ss;
        ss << "* Bullet *1*\n"
           << "* Bullet **2**";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ul><li><p>Bullet <em>1</em></p></li><li><p>Bullet <strong>2</strong></p></li></ul>"
        );
    }
    SECTION("Bullet lists with multiple paragraphs") {
        std::stringstream ss;
        ss << "* Bullet *1*\n"
           << "  Line continuation\n"
           << "\n"
           << "  extra paragraph\n"
           << "* Bullet **2**";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ul>"
                "<li><p>Bullet <em>1</em> Line continuation</p>"
                    "<p>extra paragraph</p>"
                "</li>"
                "<li><p>Bullet <strong>2</strong></p></li>"
            "</ul>"
        );
    }
    SECTION("Bullet list with code") {
        std::stringstream ss;
        ss << "* Test\n"
           << "  ```cpp\n"
           << "  code content\n"
           << "  ```\n"
           << "* Second point";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ul>"
                "<li>"
                    "<p>Test</p>"
                    "<pre><code class=\"language-cpp\">code content</code></pre>"
                "</li>"
                "<li>"
                    "<p>Second point</p>"
                "</li>"
            "</ul>"
        );
    }
    SECTION("Bullet list with code directly under bullet") {
        std::stringstream ss;
        ss << "* ```cpp\n"
           << "  code content\n"
           << "  ```\n"
           << "* Second point";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ul>"
                "<li>"
                    "<pre><code class=\"language-cpp\">code content</code></pre>"
                "</li>"
                "<li>"
                    "<p>Second point</p>"
                "</li>"
            "</ul>"
        );
    }
}

TEST_CASE("Ordered lists", "[markdown]") {
    SECTION("Plain numbered lists") {
        std::stringstream ss;
        ss << "1. Bullet *1*\n"
           << "2. Bullet **2**";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            "<ol><li><p>Bullet <em>1</em></p></li><li><p>Bullet <strong>2</strong></p></li></ol>"
        );
    }
}

TEST_CASE("Full reference file", "[markdown]") {
    std::stringstream ss;
    ss << R"(# Header
## Header 2
### Header 3
#### Header 4
##### Header 5
###### Header 6

**Bold**, *italic*, ***bold italic***

**`bold code`**
`**code with stars**`

```
full code block
```

> Quote


> Multiline
> quote

# Header with `code` **bold**

It upsets me how few markdown implementations actually support syntax in headers.
Also, this is a paragraph split across multiple lines with no formatting.

[This is a link](//example.com)
)";

    std::string expected = R"(<h1>Header</h1>)"
        R"(<h2>Header 2</h2>)"
        R"(<h3>Header 3</h3>)"
        R"(<h4>Header 4</h4>)"
        R"(<h5>Header 5</h5>)"
        R"(<h6>Header 6</h6>)"
        R"(<p><strong>Bold</strong>, <em>italic</em>, <strong><em>bold italic</em></strong></p>)"
        R"(<p><strong><code>bold code</code></strong>)"
        R"( <code>**code with stars**</code></p>)"
        R"(<pre><code>full code block</code></pre>)"
        R"(<blockquote><p>Quote</p></blockquote>)"
        R"(<blockquote><p>Multiline quote</p></blockquote>)"
        R"(<h1>Header with <code>code</code> <strong>bold</strong></h1>)"
        R"(<p>It upsets me how few markdown implementations actually support syntax in headers. )"
        R"(Also, this is a paragraph split across multiple lines with no formatting.</p>)"
        R"(<p><a href="//example.com">This is a link</a></p>)";

    REQUIRE(
        violet::Markdown::parse(ss)
        ==
        expected
    );
}

