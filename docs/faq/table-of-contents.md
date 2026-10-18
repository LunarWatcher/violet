---
{
    "title": "Adding a Table of Contents"
}
---
# Adding a Table of Contents

Only markdown pages currently get auto-generated table of contents. For HTML files, it is just an empty string.

To include a Table of Contents on a page, use `{{ page.table_of_contents }}`.

Because HTML pages aren't parsed by violet, violet cannot provide a ToC for non-markdown pages. If you have an HTML page that needs a ToC, either write it by hand, or consider changing it to a markdown file.

## Related config options

`toc-start-level` can be used to define the minimum header level that is included in the Table of Contents. Its default value is 2, and means level 1 headers are always excluded. For h1s to be part of the ToC, set
```json
{
  "toc-start-level": 1
}
```

in your `violet.json`.
