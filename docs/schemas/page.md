---
{
    "title": "The page global variable"
}
---
# The `page` global variable

The `page` global variable represents data from the frontmatter. The schema for it can be found [here](Frontmatter.md).

The frontmatter schema is used when you write the schema itself, while the `page` schema is the schema used to interact with the data you defined.

The frontmatter schema is altered slightly, notably due to dates being parsed before being made available to the page. The altered schema is:

```jsonc
{
    // String. Always present
    "title": "The page title",
    // String. Always present.
    "type": "the page type",
    // String. Always present. if no layout is in use, this holds the value "none"
    "layout": "the page layout",
    // int or omitted. Seconds since the UNIX epoch
    "date": 1234,
    // int or omitted. Seconds since the UNIX epoch
    "last_modified": 4567,
    // boolean. Always present.
    "hidden": false,
    "taxonomies": {
        // "key": ["item"] pair. Identical to the frontmatter spec. Example:
        "tags": ["cpp", "anti-ai"]
    },
    // String. Always present. Contains the page's table of contents. This is only defined for markdown files.
    // For non-markdown files, and markdown files where no headers exist in the ToC, this string is empty.
    // This is an HTML string meant for embedding onto the page. Correct use is simply using `{{ page.table_of_contents }}`.
    "table_of_contents": "string or empty",
    // The absolute URL to this page
    "url": "/path/to/the/doc.html",
    // String or omitted. User-provided.
    "summary": "User-provided summary",
    "data": {
        // data object identical to what was provided in the frontmatter
    }
}
```

## Differences

This is a summary of the differences from the frontmatter schema.

### Added

* `page.url` (derived from filename and path)
* `page.table_of_contents` (generated or empty)

### Changed

* `page.date`: source type is an ISO-8601 string, `page.date` is an int representing seconds since the epoch
* `page.last_modified`: source type is an ISO-8601 string, `page.last_modified` is an int representing seconds since the epoch

### Removed

* `page.listing`: This object is only used to configure the listing at render time, and is not available afterwards. For the actual render information, use the [global `listing` variable](listing.md) instead.

