---
{
    "title": "Violet - another static site generator"
}
---

# Violet - another static site generator

[Violet](https://codeberg.org/LunarWatcher/violet) is a C++-based, minimal static site generator. This website contains the documentation for it - and also a proof-of-concept, as this entire site is generated with violet.


## Features

* Is and will remain AI slop-free
* Designed to take advantage of markdown features rather than reinventing them with SSG-specific templates for no reason. This improves the writing experience, since the URLs can be standard markdown, and work both in the local editor and after generation.
* Generally designed to have trivially understandable behaviour
* Most of what you'd expect from a static site generator
  * Templates (through the jinja-inspired library [inja](https://github.com/pantor/inja/))
  * Markdown to HTML converter, based on a custom markdown parser with a few minor differences from major implementations, largely down to strictness. Markdown is a markup language, and in this implementation, invalid markup is an error.
    * Frontmatter in violet is JSON-based
  * Paginated page lists allow for long lists to be spread across pages
  * File iteration functions let you make your own custom lists if the built-in lists don't work for your use - but these do not paginate. The same functions are used to generate RSS or Atom feeds.
* Common sense path translations so stuff ends up where you'd expect

  One of my greatest annoyances I had with hugo (back before it turned to AI slop) was the incredible complexity required to figure out which god damn template file is being used to render this one particular file, and where the actual fuck my files ended up being placed. Hugo tries to abstract a lot of its file generation away with templating that should make it easy to work with, but that end up doing the exact opposite. Violet has therefore been intentionally designed to avoid the lookup hell that is hugo's design
  * There are up to 4 possible lookup locations for templates - 2 if you don't use a theme. This number will never increase.
    * Lookup semantics are well-defined based on a `type` and a `layout`, both of which are configurable in frontmatter. File names, folder names, or other path components will never influence the lookup location.
    * Violet trades potentially more verbose frontmatter under extremely specific scenarios for the ability to actually understand what violet does when generating a site.
  * Beyond converting `.md` to `.html`, and converting to a more URL-friendly format (spaces become -, letters are lower-cased, and non-ascii chars are stripped), no major changes are made. If you want a specific path structure, you need to provide that in the source medium.
  * Your folder format affects the templates, but because you're in control of most of the methods for handling the file tree, this doesn't violently break violet.
  * Non-automated Atom and RSS feeds that can be arbitrarily composed, so you don't have to wonder if/where violet put the feed. Means more manual setup the first time you make an RSS or atom feed, but means you also get to control precisely what goes into

### Goals

Markdown, in spite of being a portable format, isn't really that portable. There are so many implementations of markdown with differences between them that something that renders in one may not render in another. Funnily enough, this is also the case with violet, as a custom markdown parser had to be implemented to account for the mostly non-existence of markdown parsers in C++.

This is before accounting for SSG-specific features. Some SSGs force their way into markdown files notably in URLs, where using an SSG-specific command of some kind is often functionally required for URLs to not break after page generation.

Non-portable behaviour is unavoidable as long as there aren't actual standards, but the goal of this generator is to minimise the non-portable bits as much as possible. Page-relative URLs are modified in-place during generation, so a URL that works in your editor will work identically after generation. This is also enforced by the file structure; all content is relative to the page root, both statically copied and otherwise. This means there's no unnecessary translations between different kinds of content.

## How to read the documentation

This section contains general information about the documentation itself. Failure to read this will likely result in the documentation being misunderstood. Spending a few minutes to read it now will likely save you a lot of time down the line.

### Violet uses json, but the documentation uses jsonc

Json is not the best format for configuration, largely because of the lack of comments, but this limitation isn't really as bad for these kinds of applications. The technical reason for why json is used anyway is that there are very few alternative *parser implementations* in C++. There is a YAML library, but it has some severe problems, and that was before it too fell to AI slop. There are far more JSON implementations, however, and through [inja](https://github.com/pantor/inja/), we're already using a JSON library (nlohmann/json; currently slop-free). Therefore, JSON *is* the more convenient option.

Comments are nice, sure, but they can always be substituted by a decent commit history and decent documentation in the tool. However, the one exception to this is in said decent documentation, where the JSON may need to be directly commented.

As a consequence, while violet's config files are comment-free JSON, the documentation uses jsonc to be able to explain things inline. If you copy-paste config, you must remove the comments before running, or violet will error out.

## Getting started

See [Getting started](/Getting started.md)


