#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Footnotes should work") {
    SECTION("Simple footnotes") {
        std::stringstream ss;
        ss << R"(This is text[^1]

[^1]: This is, in fact, **text**)";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p>This is text<sup><a href="#fn-1">1</a></sup></p>)"
            R"(<div id="violet-footnotes">)"
            R"(<h2>Footnotes</h2>)"
            R"(<ol><li id="fn-1"><p>This is, in fact, <strong>text</strong></p></li></ol>)"
            R"(</div>)"
        );

    }

    SECTION("Multi-paragraph footnote") {
        std::stringstream ss;
        ss << R"(This is text[^1]

[^1]: This is, in fact, **text**.

  And this is a second paragraph)";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p>This is text<sup><a href="#fn-1">1</a></sup></p>)"
            R"(<div id="violet-footnotes">)"
                R"(<h2>Footnotes</h2>)"
                R"(<ol><li id="fn-1">)"
                    R"(<p>This is, in fact, <strong>text</strong>.</p>)"
                    R"(<p>And this is a second paragraph</p>)"
                R"(</li></ol>)"
            R"(</div>)"
        );
    }

    SECTION("Mixed formatting fully block-compatible footnotes") {
        std::stringstream ss;
        ss << R"(This is text[^1]

[^1]: This is, in fact, **text**. As a cursed extension of how my footnotes are parsed, this should also allow
  ```python
  print("An entire god damn code block")
  ```)";
        REQUIRE(
            violet::Markdown::parse(ss)
            ==
            R"(<p>This is text<sup><a href="#fn-1">1</a></sup></p>)"
            R"(<div id="violet-footnotes">)"
                R"(<h2>Footnotes</h2>)"
                R"(<ol><li id="fn-1">)"
                    R"(<p>This is, in fact, <strong>text</strong>. )"
                    R"(As a cursed extension of how my footnotes are parsed, this should also allow</p>)"
                    R"(<pre><code class="language-python">print("An entire god damn code block")</code></pre>)"
                R"(</li></ol>)"
            R"(</div>)"
        );
    }

}
