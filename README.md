# violet

A bare minimum static site generator

## Why?

All the good SSGs have turned to AI slop, and migrating between SSGs is a pain in the ass, so I'm rolling my own that'll remain slop-free forever.

## Features

* Most of what you'd expect from a static site generator
  * Templates (through the jinja-inspired library [inja](https://github.com/pantor/inja/))
  * Markdown to HTML converter, based on a custom markdown parser with a few minor differences from major implementations, largely down to strictness. Markdown is a markup language, and in this implementation, invalid markup is an error.
    * Frontmatter in violet is JSON-based
  * Paginated page lists allow for long lists to be spread across pages
  * File iteration functions let you make your own custom lists if the built-in lists don't work for your use - but these do not paginate. The same functions are used to generate RSS or Atom feeds.
* Common sense path translations so stuff ends up where you'd expect

  One of my greatest annoyances I had with hugo (back before it turned to AI slop) was the incredible complexity required to figure out which god damn template file is being used to render this one particular file, and where the actual fuck my files ended up being placed. Hugo tries to abstract a lot of its file generation away with templating that should make it easy to work with, but that end up doing the exact opposite. Violet has therefore been intentionally designed to avoid the lookup hell that is hugo's design[^1]
  * There are up to 4 possible lookup locations for templates - 2 if you don't use a theme. This number will never increase.
    * Lookup semantics are well-defined based on a `type` and a `layout`, both of which are configurable in frontmatter. File names, folder names, or other path components will never influence the lookup location.
    * Violet trades potentially more verbose frontmatter under extremely specific scenarios for the ability to actually understand what violet does when generating a site.
  * Beyond converting `.md` to `.html`, and converting to a more URL-friendly format (spaces become -, letters are lower-cased, and non-ascii chars are stripped), no major changes are made. If you want a specific path structure, you need to provide that in the source medium.
  * Your folder format affects the templates, but because you're in control of most of the methods for handling the file tree, this doesn't violently break violet.
  * Non-automated Atom and RSS feeds that can be arbitrarily composed, so you don't have to wonder if/where violet put the feed. Means more manual setup the first time you make an RSS or atom feed, but means you also get to control precisely what goes into


## Requirements

* C++23

### Additional requirements for serving

> [!note]
> Serving is months out. Generating takes higher priority, and this is already a tangent on top of a tangent, so this project is fairly low priority.

Violet has a built-in server for local previews based on [magpie](https://codeberg.org/LunarWatcher/magpie). The serve command is disabled by default to allow windows builds, and to allow violet to me embedded in other servers. If enabled, magpie's dependencies are added to the list.

Due to limitations of the underlying socket library at the time of writing, magpie and by extension the `serve` command are Linux-only, and therefore require linux to work.

## Documentation

See the docs folder, or https://lunarwatcher.codeberg.page/violet/

## Extended usage example

My website is built on violet, and also doubles as an example of a proper non-documentation site built on violet. Its source code is available [here](https://codeberg.org/LunarWatcher/pages). Relatively mediocre design because I'm not a designer, but that doesn't affect the parts that involve violet-specific features.

## License

Violet itself is licensed under the [MIT license](LICENSE). The documentation is licensed under [CC-By-SA 4.0](docs/LICENSE)

[^1]: Hugo's absolutely moronic lookup table nearly caused me to make something like violet on multiple occasions. I cannot overstate how incredibly infuriating hugo is to work with due to its lookup rules
