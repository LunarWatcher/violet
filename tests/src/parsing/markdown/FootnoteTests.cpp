#include "violet/conf/MarkdownConfig.hpp"
#include "violet/parsing/Markdown.hpp"
#include <catch2/catch_test_macros.hpp>
#include "parsing/markdown/fixture/MarkdownFixture.hpp"

TEST_CASE_METHOD(tests::MarkdownFixture, "Footnotes should work") {
    SECTION("Simple footnotes") {
        REQUIRE(
            render(
                R"(This is text[^1]

[^1]: This is, in fact, **text**)"
            )
            ==
            paragraph({
                "This is text",
                fnref(1, 1)
            })
            + footnotes(
                {
                    {
                        .renderedContent = paragraph({
                            "This is, in fact, ",
                            bold("text")
                        })
                    }
                }
            )
        );
    }
    SECTION("Multiple simple footnotes") {
        REQUIRE(
            render(
                R"(This is text[^1][^2]

[^1]: This is, in fact, **text**
[^2]: And this is a second footnote)"
            )
            ==
            paragraph({
                "This is text",
                fnref(1, 1),
                fnref(2, 1)
            })
            + footnotes({
                    { .renderedContent = paragraph({"This is, in fact, ", bold("text")}) },
                    { .renderedContent = paragraph("And this is a second footnote") },
                })
        );
    }

    SECTION("Multi-paragraph footnote") {
        REQUIRE(
            render(
        R"(This is text[^1]

[^1]: This is, in fact, **text**.

  And this is a second paragraph)"
            )
            ==
            paragraph({
                    "This is text",
                    fnref(1, 1)
                })
            + footnotes({
                    {
                        .renderedContent =
                        paragraph({
                                "This is, in fact, ",
                                bold("text"),
                                ".",
                            })
                        + paragraph("And this is a second paragraph")
                    }
                })
        );
    }

    SECTION("Mixed formatting fully block-compatible footnotes") {
        REQUIRE(
            render(
                R"(This is text[^1]

[^1]: This is, in fact, **text**. As a cursed extension of how my footnotes are parsed, this should also allow
  ```python
  print("An entire god damn code block")
  ```)"
            )
            ==
            paragraph({
                "This is text",
                fnref(1, 1)
            })
            + footnotes({
                    {
                        .renderedContent = paragraph({
                                "This is, in fact, ",
                                bold("text"),
                                ". As a cursed extension of how my footnotes are parsed, this should also allow"
                            })
                        + code("print(\"An entire god damn code block\")", "python")
                    }
                })
        );
    }

}

TEST_CASE_METHOD(tests::MarkdownFixture, "The footnote header toggle should switch to non-text builtin rendering") {
    REQUIRE(
        render(
            R"(Fnref[^test][^test]

[^test]: content
)",
            violet::MarkdownConfig {
                .footnoteHeaders = false
            }
        )
        ==
        paragraph(
            std::format("Fnref{}{}", fnref(1, 1), fnref(1, 2))
        )
        + footnotes(
            {
                { .renderedContent = paragraph("content"), .usages = 2 }
            },
            false
        )
    );
}

