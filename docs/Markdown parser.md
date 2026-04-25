---
---
# Markdown parser

Because no good markdown parsers seem to exist in C++, this project has rolled its own, with some major limitations that may or may not make the markdown technically non-compliant.

## Frontmatter

Frontmatter is, annoyingly (and conveniently in this case), not part of the markdown spec. Therefore, violet implements a non-standard form based on JSON, for the reasons mentioned in the documentation README:

```markdown
---
{
    // standard JSON object goes here
}
---
```

It's still delimited by `---`, as this allows for more explicit separation of the content and the frontmatter. For the actual schema, see [Frontmatter](Frontmatter.md)

## Header IDs

Header IDs are exclusively generated, and are picked by stripping all non-letter and non-numeric characters, lower-casing what's left, and replacing spaces with a `-`. The header for this section, if processed by violet, would get the ID `header-ids`. If this results in no characters included in the ID, for example if enough non-english characters are included, no ID is generated.

In the event of an ID conflict, a counter is added. For example, if `header-ids` was about to get duplicated, `header-ids-1`, `header-ids-2`, etc. would be used for every subsequent instance.

## Footnotes

Footnotes are supported, because I really like footnotes[^1]. These follow the fairly footnote style that I don't know the exact origin of:
```markdown
text[^1]

[^1]: Footnote content
```


## Blockquotes

Blockquotes use standard markdown:

```markdown
> This is a quote
> > This is a nested quote

> This is a separate quote
>
> This is a continuation line
```

> This is a quote
> > This is a nested quote

> This is a separate quote
>
> This is a continuation line

### Callouts

Callouts are also supported. All the GitHub types are supported, as well as the extra `info` callout type. Note that these are strictly typed, and using an invalid callout type is a compile error.
```markdown
> [!note]
> Content

> [!info]
> Content

> [!tip]
> Content

> [!important]
> Content

> [!warning]
> Content

> [!caution]
> Content
```

This is how they appear with the default theme:

> [!note]
> Content

> [!info]
> Content

> [!tip]
> Content

> [!important]
> Content

> [!warning]
> Content

> [!caution]
> Content

Note that unlike other implementations, all the parser does is provide a callout type and a callout title paragraph object. The styling has to be manually implemented.


[^1]: I don't go off on tangents that necessitate footnotes, you go off on tangents that necessitate footnotes >~>

