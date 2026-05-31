---
{
    "title": "File operation functions"
}
---
# File operation functions

{{ page.table_of_contents }}

## `listPages(object, location)`

This function recursively lists pages from the provided `location`. The return value is a flat list of frontmatters (see [the frontmatter schema spec](/schemas/Frontmatter.md)).

### Arguments

#### `object`

Either `site` or `page`. This is used to get the root path.

#### `location`

A location relative to the provided `object`. For example:

* `listPages(site, ".")`: Lists pages relative to the site root. Useful for listing out the entire site
* `listPages(page, ".")`: Lists pages relative to the folder the current page is in. Useful if you only care about subtrees from a given page
* `listPages(site, "some/specific/folder")`: lists the content of a specific folder. Can be useful for certain manual lists.


## `treePages(object, location)`

Similar to `listPages`, but returns an object in the following format:
```json
{
    "items": [ { ...frontmatter } ],
    "subfolders": {
        "subfolder_name": {
            "items": [ { ...frontmatter }],
            "subfolders": { ... }
        },
        "subfolder2_name": { ... }
    },
    "index": { ... optional frontmatter },
}
```

Where:
* `items` is an array of the pages for the provided context, with the exception of `index.html` (index.md/README.md)
* `subfolders` is a dict of subfolders in the provided context. The values are more of the same object currently being described
* `index`: `index.md`/`README.md`'s frontmatter, or null if the section doesn't have an index file.

This function can be used when hierarchy is important.

### Arguments

#### `object`

Either `site` or `page`. This is used to get the root path. 

#### `location`

A location relative to the provided `object`. For example:

* `treePages(site, ".")`: Lists pages relative to the site root. Useful for listing out the entire site
* `treePages(page, ".")`: Lists pages relative to the folder the current page is in. Useful if you only care about subtrees from a given page
* `treePages(site, "some/specific/folder")`: lists the content of a specific folder. Can be useful for certain manual lists.

