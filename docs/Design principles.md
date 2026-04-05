---
{ "title": "Design principles" }
---
# Design principles

This document describes the design principles behind violet. This is purely technical documentation, and is only worth reading if you want to contribute or you're just generally interested in the technical parts of violet.

## Explicit behaviour

Every page has a type and a class, and these two must provide an absolute source of what the page is.

Though it requires more verbose frontmatter, config is cheap; spending hours trying to guess which template is used for one specific thing because it can be looked up by type, class, location, time of day, the weather, the filename, and whether or not you're sitting precisely 48.169cm above the floor at the time of the compile is time I've spent that I cannot recover.

The worst-case lookup for violet is 4 locations:

* `{project}/_templates/{type}/{layout}.inja`
* `{theme}/_templates/{type}/{layout}.inja`
* `{project}/_templates/_default/{layout}.inja`
* `{theme}/_templates/_default/{layout}.inja`

Failure to match a `{layout}` in one of these four locations is an error. If you do not specify a type, but want to render it, your templates must support it explicitly. This makes both lookup behaviour explicit, and it makes it trivial to resolve which template is going to be used.

If no theme is set on a given page, this decays to just two lookup locations:

* `{project}/_templates/{type}/{layout}.inja`
* `{project}/_templates/_default/{layout}.inja`

In an older version[^1] of hugo, the worst-case was not 4, not 5, not 6, but [***46 lookup locations***](https://web.archive.org/web/20220413033808/https://gohugo.io/templates/lookup-order/). Good luck checking all 46 and correctly guessing when there's a fixed path component vs. when there's a replaceable value that you have to replace for the value to be correct.

This is the kind of mental overhead nightmare I'd like to avoid. Again, text is cheap, and the time spent looking at and reasoning about up to 4 locations is negligible compared to looking up 46 separate locations. It may not feel like it when the same `type` and `layout` is used in many files, but it will if you'd like to change a template for a built-in feature that you don't know the layout for. Taxonomies are a great example of this; the taxonomy list is a layout (`taxonomy.inja`), and the taxonomy list can be an explicit page with this specified, along with a separate `type` if you have multiple types of taxonomies.

This means that categories do not carry any weight. If you have a `blog/` folder, it alone carries no meaning. Every post within must have a
```markdown
---
{ "class": "blog" }
---
```

If you want blog posts specifically to have a special layout. Again, this is verbose, but it's trivial to automate for migration, and it's explicit about behaviour. You'll know what pages you have and that you put in a specific non-`_default` type, and it's much easier to work with that than hugo's formerly 46 lookup locations. The annoyance of verbosity is quickly outweighed by the ability to do anything without being crushed by the number of places to put a template, that may or may not accidentally affect other pages.


[^1]: as far as I can tell, they have simplified in a later version; not sure when
