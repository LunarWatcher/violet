# Project structure

Unlike many SSGs, violet does not require a rigid structure out of the box, with the exception of for templates and partials.

## Content folders

Instead, you must explicitly declare your folders through configuration:
```jsonc
{
    "include": [
        {
            // Note: paths must start with ./, and cannot contain /../
            // Paths are always relative to the root so they can be predictably mapped to non-violet
            // renders in the average editor
            "path": "./folder/",
            // Raw: Copied as-is to the output folder. This is what you want to use for asset folders. "As-is" is with
            //      the exception of JS and CSS, which may be minified in production mode.
            // Content: Folders of markdown and HTML that serve some form of parsed content
            "type": "raw|content"
        }
    ]
}
```

This is with the exception of the root folder of the project, which is always included as `content`. Note that "root folder of the project" refers to the folder containing `violet.json`

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
