# Frontmatter schema

```markdown
---
{
    "title": "Title for the page. Optional; defaults to the filename",
    "type": "Type for the page. Default: _default",
    "layout": "The layout to use for the page. Default: single_page",
    "date": "ISO 8601 datetime for when the page was published; optional, defaults to when the page was last modified. Note that fractional timezones are not supported.",
    "last_modified": "ISO 8601 datetime for when the page was last modified. If not set, it's equal to the `date`",
    "summary": "Brief description usually used in a <meta name="description"/> tag. Should be <=155 characters",
    // Whether or not the page is hidden from file iteration methods.
    // This should be set for 404.html (Codeberg Pages, GitHub pages), not_found.html (neocities), and similar.
    // Can also be used if you want a draft system on a blog.
    "hidden": false,
    "taxonomies": {
        "tags": ["tag1", "tag2", "...", "tagN"],
        "series": ["Series name"],
        "...": [...]
    },
    // Only supported if type == page_list. See Page lists#Paginated page lists for more info
    // A non-null listing enables pagination of the page.
    // enables the global `listing` variable
    "listing": {
        // The default value is 50; it must be strictly greater than 0, and less than the size of a 64 bit unsigned int (that said,
        // bugs caused by absurdly high page counts will not be fixed. Use unpaginated lists instead)
        "page_size": 50
    },
    "data": {
        // optional userdata. Can be referred through {{ page.data }} in templates. Anything non-standard goes here
    }
}
---
```

