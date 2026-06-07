# Taxonomies

Taxonomies are partly implemented. They're declared [in a page's frontmatter](/schemas/Frontmatter.md), and does not require any additional config.

Taxonomy names are only allowed to contain the characters `[a-z0-9_\-]`. Taxonomy values are currently unrestricted.

## Standard taxonomies

Though there's no technical requirements for what taxonomies are implemented or have to be used, there are currently two taxonomies that are suggested by default:

* `tags`: Used for categorising pages
* `series`: Used to link sequential posts together

These are not required to be supported, but are considered the standard minimum required taxonomies.

## Unsupported features

Taxonomies are primarily implemented for future compatibility. This means they currently only serve as static data objects, and don't render any pages.

Eventually, two lists will be implemented: A standard list containing all pages with a given taxonomy, and a list of all the taxonomies. Due to how much of a pain in the ass this is to store in a filesystem, this has been left as an exercise for future me.

Some notable issues:

* There are technically two lists, a `page_list` and a (currently non-existent) `taxonomy_list`

  This is more of an annoyance, but adding a whole other kind of page required to support taxonomies makes it harder to incrementally add pages.
* The arbitrary content of the taxonomy values makes for complicated URLs. The value cannot be restricted without screwing over non-english content, or making the encoding of the URLs a whole thing.

  This is likely the single biggest challenge. Shy of manually defining every taxonomy with a proper name, which scales awfully. Using numeric indices or similar cannot be done in a stable way in a static site generator. 

  Not having access to a database to keep things stable means name mapping is the only way to keep things stable.

