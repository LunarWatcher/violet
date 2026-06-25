---
{
    "title": "RSS and atom feeds"
}
---
# RSS and Atom feeds

Unlike in most static site generators, violet does not _directly_ come with facilities for RSS and atom feeds linked to other features. Instead, you have to manually define and declare RSS and/or Atom feeds yourself.

One of the biggest annoyances I've had with other static site generators is that the feeds just magically disappear. You can maybe get a URL from it that you can put into the frontmatter, but this can change if/when the static site generator changes. Instead, violet treats RSS and Atom feeds (or more broadly, files with the extensions `.rss`, `.atom`, and `.xml` with frontmatter) as a special type of file. This means you know precisely where the feeds on your site are, because you've declared them.

## Lookup locations

RSS and Atom feeds get a bit of special treatment since it's a whole other content type.

For the most part, the logic is the same as for HTML and markdown files, which is described in [the lookup order docs](/Project structure.md#lookup-order). However, they have two differences:

1. If no `layout` is defined, `feed` is used instead of `single_page`
2. If no matching layouts exist, the feed is parsed standalone. In other words, feeds are allowed to have no template base.

## Example (Atom feed, no base template)

TBA
