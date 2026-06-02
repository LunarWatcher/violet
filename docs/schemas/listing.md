---
{
    "title": "The listing object"
}
---
# The `listing` object

The `listing` object is a metaobject that contains information about pagination, and serves as a namespace for page interation when operating on a paginated listing page.

The `listing` object is available exclusively in paginated page lists. For non-paginated page lists, use [`listPages`](/templating/files.md#listpagesobject-location) or [`treePages`](/templating/files.md#treepagesobject-location).

## Schema

```jsonc
{
    // the current page
    "page": 123,
    // The total number of pages (shocking, I know)
    "total_pages": 234,
    "pages": [
        { ... frontmatter, see the frontmatter schema }
    ],
    // The path to the listing folder. Can be used to construct URLs
    "base_path": "path/to/listing/folder"
}
```

