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

### Paginated page lists

Paginated page lists are a subset of page lists where pagination is enabled. Unlike page lists, this comes at the loss of control over how the pages are retrieved, and instead relies on standard methods for getting the pages. This primarily makes paginated page lists suitable for blogs or similar, where the list of pages may increase infinitely.

Paginated page lists are declared similarly to standard page lists, but are used differently:

```markdown
---
{
    // we use the same layout, as the pagination is turned on via config options
    "layout": "page_list",
    // Non-null listing enables paginated page lists
    "listing": {
        // The default value is 50; it must be strictly greater than 0, and less than the size of a 64 bit unsigned int (that said,
        // bugs caused by absurdly high page counts will not be fixed. Use unpaginated lists instead)
        "page_size": 50,
    }
}
---
```

On paginated page lists, the pages are available under the [global `listing` variable](/schemas/listing.md).

## Writing page list templates

See [Templating](/templating/README.md)

