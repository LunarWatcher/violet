---
---
# Project structure

## Content folders

Content folders start at the project root and includes everything within the project.

Only `.md` and `.html` files with frontmatter are parsed by violet; everything else is copied verbatim. This means that, starting at violet's root folder, the workspace links map 1:1 with output links.

The site prefix may change this, but everything is still relative to some root, so it should still work out.

Undesirable folders can be excluded through `violet.json`; see [Config](Config.md)

### Special file names

* `README.md`: maps to `index.html` when rendered.
* `index.md`: maps to `index.html`. When both `README.md` and `index.md` exist in a folder, **`README.md` is ignored** in favour of `index.md`. This can be useful in project roots, if you have a need to separate the README on a code forge from the index page on a git repo.

## Templates

Since templates form the basis of any page, they're treated differently from the content folders. The `template` folder is **required**, and has the following structure:

```
project-root/
    |- templates
        |- _default
            |- single_page.inja
            |- page_list.inja
        |- custom
            |- base.inja
```

### Reserved folders
Folders starting with `_` are reserved for internal use. If your templates folder contains an underscored folder with an unknown name, the build will fail.

The currently used special folders are:

* `_default`: The default template used when no `type` is specified.

### Template types

The names of the inja files have special meanings. The currently used files are:

* `single_page.inja`: Used for individual pages
* `page_list.inja`: Used for list pages, i.e. pages that list out other pages in the current directory. These also provide foundations for feeds.

### Lookup order

There is no lookup order, and this is a feature. Many other SSGs have complex lookup rules that end up being a pain in the ass to follow.

* If no `type` is declared in the frontmatter, `_default` is used.
* If a non-`_default` type is used, but it doesn't contain a given template, the `_default` is sourced.
* If no template of the corresponding type nor in the `_default` fallback is found, the build fails.
* If the `type` doesn't map to a folder in `templates`, the build fails.

## Partials

Partials are just a folder of `.inja` files that can be included by other files. The folder name is `partials`, and like templates, are an immediate descendant of the project root.
