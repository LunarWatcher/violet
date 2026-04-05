---
---
# Frontmatter schema

```markdown
---
{
    "title": "Title for the page. Optional; defaults to the filename",
    "type": "Type for the page.",
    "date": "ISO datetime for when the page was published; optional, defaults to when the page was last modified",
    "last_modified": "ISO datetime for when the page was last modified. If not set, it's equal to the `date`",
    "taxonomies": {
        // Contains taxonomies, such as tags. Unused, but reserved for future use once I figure out how to not make taxonomies utter crap for DevEx
    },
    // Only supported if type == page_list. See Page lists for more info
    "listing": {
        "rss": "/path/to/rss-file.xml",
        // Whether or not the list page is recursive, i.e. whether subfolders should be included
        "recursive": false,
        // Whether or not the list page should include folders. Folders are never included in RSS generation regardless of this option,
        // but this option controls whether or not the folders are included in the list. Can be useful if you want to have a master list that
        // links to sub-lists
        "include_folders": false,
    },
    "data": {
        // optional userdata. Can be referred through {{ page.data }} in templates. Anything non-standard goes here
    }
}
---
```

