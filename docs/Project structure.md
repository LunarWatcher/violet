---
---
# Project structure

TL;DR:

* Most folders: [Content folders](#content-folders)
  * Content folders are rendered or copied to the site.
* Folders with a leading `_`: reserved for internal use, and are not exported. Currently in use:
  * [`_themes`](#themes)
  * [`_templates`](#templates)
  * [`_partials`](#partials)


## Content folders

Content folders start at the project root and includes everything within the project, unless excluded by the `exclude` [config option](Config.md).

Only `.md` and `.html` files with frontmatter are parsed by violet; everything else is copied verbatim. This means that, starting at violet's root folder, the workspace links map 1:1 with output links.

The site prefix may change this, but everything is still relative to some root, so it should still work out.

### Special file names

* `README.md`: maps to `index.html` when rendered.
* `index.md`: maps to `index.html`. When both `README.md` and `index.md` exist in a folder, **`README.md` is ignored** in favour of `index.md`. This can be useful in project roots, if you have a need to separate the README on a code forge from the index page on a git repo.

## Themes

> [!tip]
> Themes are an optional feature that can be used to provide themes reusable across multiple sites without weird linking behaviour. They're just a package of `_templates` and `_partials`. If you're not sure what to start with, you can skip themes for now, and copy the folders into a theme later if you need it.
>
> Project-local `_templates` and `_partials` always take precedence over themes, so if you want to add to a theme, you can do so locally in `_templates` and `_partials`.

The `_themes` folder contains the themes available to the project, with one notable exception: the built-in `_default` theme. Theme names with a leading underscore are reserved names, and are checked against violet's internal theme folder. The `_default` theme is meant to provide some sane defaults if you don't want to build the whole site from scratch.

For more information about themes, see [Themes](Themes.md).

The recommended way to add themes is a git submodule:
```
mkdir _themes
# Note: not a real theme, but no real external themes exist to use as an example here yet.
# (due to the risk of sloppification, this may never contain an actual theme)
git clone git@codeberg.org:example/example _themes/
```


## Templates

Since templates form the basis of any page, they're treated differently from the content folders. The `_templates` folder is **required**, and has the following structure:

```
project-root/
    |- _templates
        |- _default
            |- single_page.inja
            |- page_list.inja
        |- custom
            |- base.inja
```


The template path can be described as `_templates/{type}/{layout}.inja`. In the example above:

* `single_page`, `page_list` are built-in layout types
* `base.inja` is a custom layout types
* `custom` and `_default` are both types, with `_default` being a builtin type with a special meaning.


### Reserved folders
Folders starting with `_` are reserved for internal use. If your templates folder contains an underscored folder with an unknown name, the build will fail.

The currently used special folders are:

* `_default`: The default template used when no `type` is specified, and that serves as a fallback for all custom types that lack a specific type.

### Template types

The names of the inja files have special meanings. The currently used files are:

* `single_page.inja`: Used for individual pages
* `page_list.inja`: Used for list pages, i.e. pages that list out other pages in the current directory. These also provide foundations for feeds.

### Lookup order

There is no lookup order, and this is a feature. Many other SSGs have complex lookup rules that end up being a pain in the ass to follow.

* If no `type` is declared in the frontmatter, `_default` is used.
* If a non-`_default` type is used, but it doesn't contain a given template, the `_default` is sourced.
* If no template of the corresponding type nor in the `_default` fallback is found, the build fails.
* If the `type` doesn't map to a folder in `templates` or in the theme, if set, the build fails.


## Partials

Partials are just a folder of `.inja` files that can be included by other files. The folder name is `_partials`, and like templates, are an immediate descendant of the project root.
