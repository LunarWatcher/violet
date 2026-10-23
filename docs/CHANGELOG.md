---
{
    "title": "Changelog"
}
---
# Changelog

This page contains the changelog for violet. Please note that you'll encounter the following indicators:

* `BREAKING:` - changes that require changes as part of an update
* `CLI:` and `CLI (subcommand):` - Changes to the command line flags and commands.
* `Internal:` - changes made to the internals of violet that are observable, but don't provide anything very breaking to the workflow for the average user, or provide no observable change what so ever.
* `Markdown:` - changes made to the markdown engine
* `Templates:` - changes made to the API used by templates, i.e. referring to the violet-specific functions used in the iris templates
* `Theme(theme name):` - changes made to specific builtin themes

{{ page.table_of_contents }}


## [unreleased]

### Added

* `site.timezone` is now the default timezone when `formatDate` is called without a timezone argument
* Meta: There is now a [list for third-party themes](https://lunarwatcher.codeberg.page/violet/themes/third-party-themes.html). It is currently empty, but it does exist :P

### Changed

* BREAKING: `loadJson`'s signature has changed from `loadJson(object, "filename.json")` to `loadJson("filename.json"[, object])`.
  * Existing use of `loadJson` will continue to work, but will produce a warning in the logs. The old signature will be removed in v1
* `loadJson` no longer requires a frontmatter when using absolute file paths

### Fixed

* Fixed a bug where `loadJson` failed to load paths relative to the provided `object`.

## [v0.15.0] (2026-10-23)

### Added

* Added support for ISO dates, and ISO datetimes without timezones
  * In examples, `2026-10-23` and `2026-10-23T00:00:00` are both supported.
* Templates: added `formatDate()` for parsing non-builtin dates (i.e. datetimes that aren't in `page.date` or `page.last_modified`)
* Added `site.timezone` to the config schema. This is optional and defaults to `"Etc/GMT"` if not provided. For valid names, see [the list of tz database timezones](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones).

### Removed

* BREAKING (on paper): Removed `page.listing`. It must still be used in frontmatter to define page lists (per [Page lists](/Page lists.md)), but the exact config object is no longer available through the `page` global.
  * This is only breaking on paper since, to my knowledge, it has no practical use. All useful data should be exposed through the `listing` global, which is present when a paginated page list is rendered.
  * Again, to be clear, this does not affect the config for page lists, just that `page.listing` isn't accessible when the template is rendered.
  * As a tangent, the docs have been very unclear on the frontmatter schema vs. the `page` schema. There is now a [separate doc page for the `page` global](/schemas/page.md) with a summary of the exact data differences.

## [v0.14.0] (2026-10-20)

### Site and theme migration guide

`.inja` is superceded by `.iris`. The following automation script can be used for most standard use-cases:

```bash
# Replace the .inja extension inside templates
rg inja --files-with-matches | xargs -d '\n' sed -i 's/\.inja/.iris/g'
# Mass-rename inja files
for file in **/*.inja; do mv "$file" "${file%.inja}.iris"; done
```

No functional changes have been made in iris relative to inja, so no other migration should be necessary. Aside this change, backwards compatibility for violet consumers will be maintained. (C++ API compatibility in iris will likely break at some point, but this will not be observable by end-users of violet)

### Added

* Added proper error messages for markdown errors ([#9](https://codeberg.org/LunarWatcher/violet/issues/9) (partial)). Example from this doc site's [Editor setup](/Editor setup.md) with an invalid URL:
  ```
  21:26:42.016 | error    | SiteGenerator.cpp:231    | Markdown error in Editor setup.md
  21:26:42.016 | error    | SiteGenerator.cpp:232    | Error: Unclosed url
  Near byte:
  527
  Location:
    Note that for setting up editors not in this list, the usually only step necessary is to associate `.iris` with a syntax renderer for jinja templates. inja templates are similar enough in implementation to jinja templates that it'll mostly be correct.
  
    [link](
    ~~~~~~^
  
    ## Emacs
  
    `web-mode` supports jinja syntax, which inja is based enough on for it to be functionally identical. Adding the `.iris` extension to web-mode with the `jinja` engine allows templates to be rendered:
  ```
  * Note that the position highlight does not actually show a start position, only an end position. The `~` are just to make the `^` more obvious. Start positions may be handled in a future release
* CLI (init): Auto-generate a `.gitignore`
  
### Changed

* BREAKING (ish): Migrated from inja to [iris](https://codeberg.org/LunarWatcher/iris) (an inja fork with the same anti-AI guarantees as violet - no functional changes have been made in the fork yet, so it should not be noticeable)
  * Violet now also supports the additional `.iris` file extension. `.iris` will be kept until v1; migrating before then is recommended.
  * `.inja` legacy support is disabled by default. Run `violet generate --legacy-extensions` to let existing projects compile

### Fixed

* CLI (init): generated `publish.sh` accidentally included `$violet_exe`, which should just be `violet`. This was a copypasta oversight from copying violet's own `publish.sh`

## [v0.13.1] (2026-10-19)

### Fixed

* Fixed a segfault caused by undefined behaviour when iterating pages for pagination ([#24 p. 2](https://codeberg.org/LunarWatcher/violet/issues/24))

## [v0.13.0] (2026-10-19)

### Fixed

* BREAKING: Don't aggressively exclude `README.md` and `index.md` from pagination functions ([#24 p. 1](https://codeberg.org/LunarWatcher/violet/issues/24))
  * This may require adding `"hidden": true` To READMEs with associated feeds if the page itself directly includes `{{ listing }}` variables that aren't just part of the page's associated templates. A page that includes `{{ listing.total_pages }}` must be hidden after this change.

## [v0.12.0] (2026-10-18)

### Added

* CLI (init): Added `publish.sh` as part of the generated template site
* Markdown: Added angle bracket link support (`<https://example.com>`)
* Markdown: Include "Footnotes" in the ToC (Table of Contents) if present
* Markdown: Fix bug where headers containing links breaks ToC rendering
  * Specifically, the URLs rendered as being part of the header took precedence over the URL pointing to the associated header
* Markdown: Add new config option (`toc-start-level`) to allow skipping earlier header levels. This allows first level headers to be excluded from the ToC if you follow semantically correct HTML, and only have one h1 on a page.
  * This is largely to avoid including the main header on the page in the ToC. This can already be done if the header is moved into a template, but this isn't always going to be the case.
  * The default value is 2, so h1 headers will not be included in the ToC anymore. To restore the previous behaviour, set `"toc-start-level": 1`. 
  * The value can be set to any of the 6 legal header levels - it's only going to be useful when set to 1 (include h1) and 2 (exclude h1), but you can in theory exclusively include `<h6>`es if you have a particularly cursed site where this is useful.
    * (Supporting this does not add any complexity to the code or config, so might as well support it)

### Fixed

* Added missing include in `GenerateOpts` that may or may not break the build on certain compilers

## [v0.11.2] (2026-09-27)

### Added

* CLI (init): Help/quickstart links after `violet init` runs successfully
* Theme (default): added `<table>` lighlighting

## [v0.11.1] (2026-09-26)

### Added

* CLI (generate): Added `--root`, largely for debug and E2E test purposes

### Fixed

* Fixed a bug in list parsing that caused `* 0.10.0` to trigger an infinite parsing loop ([#17](https://codeberg.org/LunarWatcher/violet/issues/17))
* Fixed a bug where code inside an unordered list erroneously consumed the first character after the open tag ([#18](https://codeberg.org/LunarWatcher/violet/issues/18))

## [v0.11.0] (2026-09-26)

### Added

* CLI (init): Added the `init` command for quickly setting up a template repo

### Changed

* `//` and path upward traversal are no longer allowed in the prefix

### Fixed

* 0.10.0's addition of `--prefix` broke prefixes due to a CLI12 oddity
  * Technical summary: `add_option(...)->default_val(std::nullopt)` causes the value to be initialized to an empty string rather than an empty optional. This caused the prefix to always default to a string, and break all pages where `prefix != ""`.

## [v0.10.0] (2026-09-21) (RETRACTED)

> [!warning]
>
> Broken; do not use. Upgrade to v0.11.0

### Added

* CLI (generate): Added `-p,--prefix` for overriding the prefix manually. This is another alternative to `-l` meant for supplying a real path. For example, `--prefix /` can be used to override the prefix to the root level.

  The standard caveats with the prefix apply as to the config, i.e. the prefix shouldn't start with an absolute path - however, `/` is especially allowed with `--prefix`, and is equivalent to `"prefix": ""` in `violet.json`.
* Markdown: Added support for checklists (`- [ ]` and `- [x]`)

### Changed

* `prefix` is now URL-encoded
* Fixed a bug where `"prefix": "/"` caused invalid local paths or paths that otherwise start with a double slash

## [v0.9.0] (2026-08-08)

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
  * This also means `none.iris` is now an illegal template filename that will never be used.

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

<!-- TODO: This fucking _sucks_ to maintain, but I largely do it because it's
baggage from when I tried following the Keep A Changelog-standard. I don't anymore
because it's not a great format, and I _definitely_ don't now anymore, because,
of fucking course, it's AI slop:
https://github.com/olivierlacan/keep-a-changelog/blob/main/AGENTS.md
https://github.com/olivierlacan/keep-a-changelog/commit/06fff4ddc666a9751387e7e3bb02af17e03e41a0

Friends apparently let friends read an AI slop changelog instead of bothering
writing one of the single most important human-facing documents. Hypocrites
-->
[unreleased]: https://codeberg.org/LunarWatcher/violet/compare/v0.15.0...master
[v0.15.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.14.0...v0.15.0
[v0.14.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.13.1...v0.14.0
[v0.13.1]: https://codeberg.org/LunarWatcher/violet/compare/v0.13.0...v0.13.1
[v0.13.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.12.0...v0.13.0
[v0.12.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.11.2...v0.12.0
[v0.11.2]: https://codeberg.org/LunarWatcher/violet/compare/v0.11.1...v0.11.2
[v0.11.1]: https://codeberg.org/LunarWatcher/violet/compare/v0.11.0...v0.11.1
[v0.11.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.10.0...v0.11.0
[v0.10.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.9.0...v0.10.0
[v0.9.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.8.0...v0.9.0
[v0.8.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.7.0...v0.8.0
[v0.7.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.6.0...v0.7.0
[v0.6.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.5.0...v0.6.0
[v0.5.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.4.0...v0.5.0
[v0.4.0]: https://codeberg.org/LunarWatcher/violet/compare/v0.3.0...v0.4.0
[v0.3.0]: https://codeberg.org/LunarWatcher/violet/releases/tag/v0.3.0
