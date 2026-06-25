---
{
    "title": "The default theme"
}
---
# The default theme

The default theme is the (shock) default theme, that's enabled when no theme is set (and themes aren't explicitly disabled).

The theme is kept minimal, as it's largely meant for smaller doc sites or similar that don't need much personalisation. It also serves as the main theme for the violet doc page, and is therefore primarily aimed at documentation use.

The main use for the default theme is documentation websites or similar; information-dense websites where every page should be directly and centrally accessible. This is largely so violet can be trivially deployed to places where mkdocs is currently used, with the idea being that more personalised websites require more custom theming anyway.

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
            // Both name and URL are required
            "name": "LunarWatcher/violet",
            "url": "https://codeberg.org/LunarWatcher/violet",
            "license": "Optional text describing the license. <a href=\"https://example.com\">Supports HTML</a>"
        },
        // Only used by the default feed template, and only required when your site contains a `.atom` file with frontmatter
        "domain": "https://example.com",
        // Only used by the default feed template, and only required when your site contains a `.atom` file with frontmatter
        "author": {
            "name": "Your name",
            "email": "Your email"
        }
    }
}
```

## Feeds

The default theme includes a basic Atom template. To create an atom file for, for example, a blog, create `blog/feed.atom` (or equivalent where you have your blog posts) with the content:
```xml
---
{
    "data": {
        "name": "Blog posts",
        "domain": "https://example.com",
        "for-scope": "/blog/"
    }
}
---
```

The data fields are:

* `name`: The name of your feed. Only used internally in the feed.
* `domain`: The domain the feed is rendered at.
* `for-scope`: An absolute path to the root content directory of feed, i.e. the path where the content is grouped together.

> [!tip]
> If site generation fails after adding an RSS feed, you may be missing the `domain` and/or `author` keys in your `violet.json`. See [the config options for the default theme](#config-options)

RSS feeds are not part of the default theme, nor is linking to the RSS feed from the `<head>`. If you need the RSS functionality, I strongly recommend creating a new theme. Changes that automate additions to `<head>` may be added in the future, but will likely still be manual and `data`-based.
