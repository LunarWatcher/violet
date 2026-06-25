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

## `listPagesByTaxonomy(taxonomy, value)`

Equivalent to `listPages`, but scoped to a provided `taxonomy` with a `value`

For example, given the frontmatter
```markdown
---
{
    "taxonomies": {
        "tags": ["tag"]
    }
}
---
```

The following code:
```markdown
{{ listPagesByTaxonomy("series", "Series 1") }}
```

Will list the pages with the series `"Series 1"`.

Note that the taxonomy functionality is underdeveloped at this time, and this has little use outside clear article relationships. This function is primarily intended for use with the `series` and similarly scoped taxonomies, and not tags. See [Taxonomies](/Taxonomies.md) for more info.

## `paginatedUrl(base_url, page_number)`

Given a paginated URL prefix, this function returns the paginated URL for page `page_number`.

Example:
```markdown
Only valid in a paginated page list:
{{ paginatedUrl(listing.base_path, 69) }}

Valid anywhere:
{{ paginatedUrl("/blog/posts", 69) }}
```

Output:

> Only valid in a paginated page list: current/path/page/69/
>
> Valid anywhere: /blog/posts/page/69

Standard practice in URLs is to chain it with `site.prefix` (this will change in the future):
```html
<a href="{{ site.prefix }}/{{ paginatedUrl(listing.base_path, 69) }}>Text</a>"
```

## `listPagesPaginated(object, location, sortMethod[, pageSize[, pageNumber]])`

Equivalent to `listPages`, but returns up to `pageSize` entries at page `pageNumber`, as determined by the `sortMethod`. 

Returns an empty list if `pageNumber` exceeds the number of available pages.

### Arguments

#### `object`

The object the location is relative to. Usually `page` or `site`. 

#### `location`

The path relative to the provided object to iterate. 

The page number to get the pages for. 

#### `sortMethod` (enum constant: `violet.sort`)

Must be set to one of the values in the global variable `violet.sort`. See [the spec for the global `violet` variable](/schemas/violet.md#violetsort-enum-constants)

#### `pageSize`

The size of each page. Default value: 50

#### `pageNumber`

The page number to load. Default value: 1

