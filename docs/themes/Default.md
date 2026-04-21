---
{
    "title": "The default theme"
}
---
# The default theme

The default theme is the (shock) default theme, that's enabled when no theme is set (and themes aren't explicitly disabled).

The theme is kept minimal, as it's largely meant for smaller doc sites or similar that don't need much personalisation. It also serves as the main theme for the violet doc page, and is therefore primarily aimed at documentation use.

## Vendored libraries

The default theme vendors [highlightjs](https://github.com/highlightjs/highlight.js), licensed under the BSD-3-Clause license. Highlight.js is used for code block highlights, which is functionally a requirement for most sites.

## Config options

```jsonc
{
    "data": {
        // Adds a language code to the HTML
        "language": "en-GB",
        // Enables a button that links to a git repo
        "repo": {
            "name": "violet",
            "url": "https://codeberg.org/LunarWatcher/violet"
        }
    }
}
```
