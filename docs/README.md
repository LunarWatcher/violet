---
{
    "title": "How to read the documentation",
    "layout": "page_list"
}
---

# Violet - another static site generator

[Violet](https://codeberg.org/LunarWatcher/violet) is a C++-based, minimal static site generator. This website contains the documentation for it - and also a proof-of-concept, as this entire site is generated with violet.

## Features

* Is and will remain AI slop-free
* Designed to take advantage of markdown features rather than reinventing them with SSG-specific templates for no reason. This improves the writing experience, since the URLs can be standard markdown, and work both in the local editor and after generation.
* Generally designed to have trivially understandable behaviour

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

