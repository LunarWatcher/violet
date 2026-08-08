---
{
    "title": "Changelog"
}
---
# Changelog

This page contains the changelog for violet. Please note that you'll encounter the following indicators:

* `BREAKING:` - changes that require changes as part of an update
* `Internal:` - changes made to the internals of violet that are observable, but don't provide anything very breaking to the workflow for the average user, or provide no observable change what so ever.
* `Markdown:` - changes made to the markdown engine
* `Templates:` - changes made to the API used by templates, i.e. referring to the violet-specific functions used in the inja templates
* `Theme(theme name)` - changes made to specific builtin themes

{{ page.table_of_contents }}


## [unreleased]

### Added

* Templates: Added `render(frontmatterRef)`, which allows pages from the various iteration functions rendered outside their normal filesystem declaration.
  * Note: this does not replace `{% include "content" %}` due to limitations in inja. `{% include %}` is subject to context propagation, while `render()`, much like `recursiveInclude`, has to re-build the context from scratch.

    In `recursiveInclude`, this is a feature, but in `render()`, that makes it a weaker alternative (or stronger, depending on your point of view). See the docs for `render` for more information
* Theme(default): Added `render()`ed `<content>` to the default RSS feed.
* Templates: Added `urlEncode`

### Fixed

* Template preservation in paragraphs no longer results in a trailing `}` after the template

## [v0.8.0] (2026-08-06)

### Added

* Allow non-markdown, non-html, and non-asset files to be processed via the extra `.in` extension ([#14](https://codeberg.org/LunarWatcher/violet/issues/14))
  * To process a `.txt` file for example, it would need to be called `filename.txt.in`
* Markdown: `~~strike~~` ([#15](https://codeberg.org/LunarWatcher/violet/issues/15))
* `"layout": "none"` can be used to fully disable layouts.
  * This also means `none.inja` is now an illegal template filename that will never be used.

### Changed

* Assets (`.js`, `.css`, `.mjs`) are now allowed to have layouts, but the default layout is `none`.

### Fixed

* Inline templates are now skipped by the markdown renderer
* Made inja errors better describe what file the error is from

## [v0.7.0] (2026-07-26)

### Added

* Templates: added `loadJson()` for loading JSON files into variables.
* Added `_violet-meta.json`
  * This file is only generated if `"metadata": true` is set in `violet.json`, and contains metadata about the build
  * For more information, see [the file's schema](/schemas/violet-meta.md)

## [v0.6.0] (2026-07-22)

### Added

* Markdown: Better template parsing, as well as proper HTML block identification. Template _blocks_ and HTML blocks are now left alone. Previously, only individual lines with a template were handled.
* Markdown: Syntax escaping in text \o/ \[writing with brackets now works again\]

### Changed

* BREAKING: explicit filtering that removed `404.html` has been replaced with `"hidden": true` in the frontmatter. This allows for site host-agnostic 404 pages.
  * This largely just means `404.html` in custom themes needs to have `hidden: true` added if it's in a folder where there's a file iterator.

### Fixed

* Handled breaking change in CLI12

## [v0.5.0] (2026-06-25)

### Added

* Markdown: Horizontal rules are now supported (`---` separators)
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
* Theme(default): added Atom feed template

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

[unreleased]: https://codeberg.org/LunarWatcher/violet/compare/v0.8.0...master
[v0.8.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.7.0...v0.8.0
[v0.7.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.6.0...v0.7.0
[v0.6.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.5.0...v0.6.0
[v0.5.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.4.0...v0.5.0
[v0.4.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.3.0...v0.4.0
[v0.3.0]: https://codeberg.org/LunarWatcher/violet/releases/tag/v0.3.0
