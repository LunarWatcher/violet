---
---
# Config

Violet's config is stored in a `violet.json` file at the content root for the project.

This may be, but doesn't have to be the content root for a git repository. As an obligatory reminder, while jsonc is used in the documentation, `violet.json` uses non-comment-supporting json. See [the index](README.md) for more information

## Schema

```jsonc
{
    "name": "Site name",
    "description": "Optional description; can be used by templates",
    // Theme is a special key. By default, it's _default, which is a built-in theme.
    // As per the docs for Themes, themes prefixed with a _ are builtin themes
    // If omitted, the _default theme is used.
    // If explicitly set to null, no theme is loaded, and the site itself must define templates before rendering can happen.
    // Otherwise, a folder in _themes/{value of theme} must exist and be not empty.
    // Themes are not required; if you're unsure, I suggest not defining this config option and letting it _default
    "theme": "_default",
    // A path prefix for the site. This must not be a full domain. 
    // For example, if you want your site rendered at https://username.codeberg.page/repo, the site prefix would be /repo
    "site_prefix": null | str,
    // Defines a set of folders to not include in the build. This list, by default, includes
    // all files and folders that start with ".git" in the root folder (incuding .git/, .gitignore, .gitmodules, etc) through a separate exclusion mechanic.
    "exclude": [
        "list/of/folders/to/exclude"
    ],
    "data": {
        // Contains userdata. Referenced through {{ site.data }}
    }
}
```

## The `data` key

The `data` key is used to contain arbitrary data. The contents of this object are not type-checked or otherwise validated (beyond, obviously, parsing it as JSON).

The rest of the config struct is reserved for features so commonly used that any reasonable site would include it. Anything else used by an arbitrary site goes in the `data` struct instead.
