---
{
    "title": "Changelog"
}
---
# Changelog

This page contains the changelog for violet. Please note that you'll encounter the following indicators:

* `BREAKING:` - changes that require changes as part of an update
* `Internal:` - changes made to the internals of violet that are observable, but don't provide anything very breaking to the workflow for the average user, or provide no observable change what so ever.
* `Templates:` - changes made to the API used by templates, i.e. referring to the violet-specific functions used in the inja templates

{{ page.table_of_contents }}


## [unreleased]

### Added

* Horizontal rules are now supported (`---` separators)
* Allowed RSS/ATOM (`.atom`, `.rss`, `.xml`) to be passed through template rendering
* Allowed some assets (`.css`, `.js`, `.mjs`) to be passed through template rendering. This is a weaker version where no base template is used, i.e. the templates operate directly on the asset. Mainly intended to let `{{ site.prefix }}` be used in CSS files
* Templates: added the `now()` function for use with the time parsing functions. Example use: `{{ formatDate(now(), "%FT%T%z") }}`
* Templates: Added the `violet` global variable.
  * Added `violet.datetime.iso`, which is a shorthand for the `"%FT%H:%M:%OS%Oz"` date format string
  * Added `violet.sort` with two values, which is used with the `listPagesPaginated`
* Templates: Added `listPagesPaginated`, which is used for listing out pages of content.
* Redesigned and implemented the RSS strategy to require manual setup of the feeds, with some modifications in the defaults for RSS and Atom feeds' templates.
  * See [RSS and atom feeds](/RSS and atom feeds.md)
* New command: `violet version`

### Changed

* Internal: Switch from stc minilog to standalone minilog (better logging, woo)
* Internal: Moved `--debug` into a standard function to avoid duplicating logic


## [v0.4.0]

### Changed

* BREAKING: `site.site_prefix` is now `site.prefix`. You can rewrite instances with:
  ```
  # Note: your working directory must be in your project, or this
  # may have unintended consequences.
  rg site_prefix --files-with-matches | xargs sed -Ei 's/site_prefix/prefix/g'
  ```


## [v0.3.0]

Initial tracked commit. Commits prior to this were considered too unstable to bother.

[unreleased]: https://codeberg.org/LunarWatcher/violet/compare/v0.4.0...master
[v0.4.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.3.0...v0.4.0
[v0.3.0]: https://codeberg.org/LunarWatcher/violet/releases/tag/v0.3.0
