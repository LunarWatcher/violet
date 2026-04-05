---
---
# Page lists

Page lists, also called content lists, are special pages that act as the root for a feed. Though page lists typically are a `README.md`, they aren't required to.

## Declaring a page list
```markdown
---
{
    "layout": "page_list"
}
---
# Page content

The page lists can contain content of their own. Where the content is placed, or if it's included at all, depends on the template implementation.
```

No pages are forced into being page lists, as it's perfectly normal to have folders with content without the index page therefore automagically being an index page.

> [!note]
> This sources `_default/page_list.inja` as the template.

## Additional features

### RSS

If you want to generate an RSS feed for a given page list, you can opt into this with:
```markdown
---
{
    "layout": "page_list",
    "rss": "/feed.xml"
}
---
```

The value of `rss` is the path it's mounted on in the rendered view. `/feed.xml` would mean `https://example.com/feed.xml` would be generated.

## Writing page list templates

See [Templating](Templating.md)

