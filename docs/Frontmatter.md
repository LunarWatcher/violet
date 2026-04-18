---
---
# Frontmatter schema

```markdown
---
{
    "title": "Title for the page. Optional; defaults to the filename",
    "type": "Type for the page. Default: _default",
    "layout": "The layout to use for the page. Default: single_page",
    "date": "ISO datetime for when the page was published; optional, defaults to when the page was last modified. Note that fractional timezones are not supported.",
    "last_modified": "ISO datetime for when the page was last modified. If not set, it's equal to the `date`",
    "taxonomies": {
        // Contains taxonomies, such as tags. Unused, but reserved for future use once I figure out how to not make taxonomies utter crap for DevEx
    },
    // Only supported if type == page_list. See Page lists for more info
    "listing": {
        // Path to where the RSS file is rendered.
        "rss": "/path/to/rss-file.xml",
        // Whether or not the list page is recursive, i.e. whether subfolders should be included
        "recursive": false
    },
    "data": {
        // optional userdata. Can be referred through {{ page.data }} in templates. Anything non-standard goes here
    }
}
---
```

