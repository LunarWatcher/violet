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
* `index.md`: maps to `index.html`. When both `README.md` and `index.md` exist in a folder, **`README.md` is ignored** in favour of `index.md`. This can be useful in project roots, if you have a need to separate the README on a codeforge from the index page on a git repo.

### Filetypes and default layouts

* `.md`, `.html`: standard content files, defaults to `"layout": "single_page"`
* `.js`, `.mjs`, `.css`: asset files, defaults to `"layout": "none"`
* `.xml`, `.rss`, `.atom`: feed files, defaults to `"layout": "feed"`. Note that you can have `.xml` files that aren't feeds, as there are no URL translations, this is just the default assumption. For RSS and ATOM feeds, see [RSS and atom feeds](/RSS and atom feeds.md)
* `.in`: a special extension that's stripped, and indicates that the file should be template-processed. See [templating non-supported filetypes](Templating non-supported filetypes.md). Defaults to `"layout": "none"`.
* Everything else: copied verbatim without template processing.

In addition to the layout, the default `"type"` is `"_default"` for all of the above.


## Themes

> [!tip]
> Themes are an optional feature that can be used to provide layouts that can be reused across multiple sites without weird filesystem linking behaviour. They're just a package of `_templates` and `_partials`, and optionally assets. If you're not sure what to start with, you can skip themes for now, and copy the folders into a theme later if you need it.
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

Since templates form the basis of any page, they're treated differently from the content folders. The `_templates` folder is required for sites when a theme isn't in use, and has the following structure:

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

The currently recognized special folders are:

* `_default`: The default template used when no `type` is specified, and that serves as a fallback for all custom types that lack a specific type.

### Special layout names

The names of the inja files have special meanings. The currently used files are:

* `single_page`: Used for individual pages; this is also the default for most filetypes.
* `page_list`: Used for list pages, i.e. pages that list out other pages in the current directory. This is also the only type that can create a paginated list.
* `none`: Special layout name that maens no layout file is loaded. Consequently, `none.inja` is never a real file, and will not be loaded by violet. You can still name a file `none.inja` and include it in another layout, as it's not a reserved name, but `"layout": "none"` does not result in it being loaded

### Lookup orders and theme mechanics

Project-local templates are optional if a theme is used. Violet requires that the template or partial exists somewhere. This means that if you use a theme, you don't need project-local templates, unless you use something the theme doesn't support. In theory, a theme could also require that you override a partial for customization, though this is a theme-specific caveat.

Similarly, if you don't use a theme (`"theme": null`), you must provide the templates in the project-local templates, and the theme lookup locations are disabled outright.

With a theme, this is the lookup order for page templates:

* `{project}/_templates/{type}/{layout}.inja`
* `{theme}/_templates/{type}/{layout}.inja`
* `{project}/_templates/_default/{layout}.inja`
* `{theme}/_templates/_default/{layout}.inja`

Without a theme, it's just the two `{project}` lookup locations that are checked.

Violet does not require that any particular files exist - only that _some_ file exists. If no templates match a given file, the file will fail to render and an error will be shown.

## Partials

Partials are just a folder of `.inja` files that can be included by other files. The folder name is `_partials`, and like templates, are an immediate descendant of the project root or theme root.

### Lookup order

Similarly to templates, partials are checked with a lookup order that prioritizes the project-local partials first. This makes it possible for a theme to allow overrideable partials if desired.

* `{project}/_partials/{user-provided path}`
* `{theme}/_partials/{user-provided path}`

The `{user-provided path}` is just whatever was provided to the `{% include "partials/path.inja" %}` statement. Note that `.inja` may be omitted from the include statement, in which case it'll be appended.

