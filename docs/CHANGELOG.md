---
{
    "title": "Changelog"
}
---
# Changelog

## [unreleased]

### Added

* Horizontal rules are now supported (`---` separators)
* Allowed RSS/ATOM (`.atom`, `.rss`, `.xml`) to be passed through template rendering
* Allowed some assets (`.css`, `.js`, `.mjs`) to be passed through template rendering. This is a weaker version where no base template is used, i.e. the templates operate directly on the asset. Mainly intended to let `{{ site.prefix }}` be used in CSS files

### Changed

* Internal: Switch from stc minilog to standalone minilog (better logging, woo)


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
