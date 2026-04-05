---
{ "title": "Themes" }
---
# Themes

Themes are a special construct meant to be more reusable than directly writing templates and partials. These are primarily meant to be included as submodules in a project.

Themes are not required, and work together with templates and partials in the project. When a template or partial is required, the lookup order is as follows:

1. The project's `_templates` and `_partials` are checked.
2. If no match is found and a theme is set, look in its `_templates` and `_partials` folders.

## The themes folder

The `_themes` folder, partially described in [Project structure](Project structure.md), can on paper contain multiple themes. However, only one theme can be used at a time, so it should only contain one theme. This is not enforced in any way, as it makes no practical difference aside maybe making cloning the repo slower.

## The structure of a theme

Themes are really just special sites in a trenchcoat. In theory, you can include any site, provided you can get the violet root mounted to `_themes/theme_name`.

A theme is just a bundle that contains a `_templates` and optionally `_partials`. This also means that a theme can be a self-describing site generated with itself. Since the project structure of a theme is 1:1 with that of a site, the theme can directly reference its own partials without needing any nasty hacks. However, themes get an additional special file: `violet.theme.json`.

### The theme config file (`violet.theme.json`)

The theme config file describes metadata about the theme that cannot otherwise be inferred. This includes which folders to mount for assets. This is the only config file that's used by the theme.

```jsonc
{
    // Defines which folders of the theme to mount to the page. Like proper violet sites,
    // these folders are mounted directly under the root (or site_prefix if set).
    // As a consequence, you must disambiguate the names of your static assets.
    // DON'T: assets/style.css
    //      This will almost certainly conflict with the user's extra features
    //
    // DON'T: my_theme/style.css
    //      This can be fine in very specific circumstances, but can be weird if your theme has a generic name.
    //      Naming is one of the hardest things in software, so rather than guessing, it's recommended
    //      to simply avoid this convention.
    //
    // DO: assets/my_theme/style.css
    //      This namespaces the style in `assets` and in a theme-specific subfolder.
    //      This is the option that's the most explicit in terms of origin.
    //      It my still conflict, but with warnings about exported folders, the user gets
    //      the best chance to spot their own errors if this approach is used.
    //
    // DO: assets/my_theme_style.css
    //      This namespaces the style in `assets`, which helps reduce the amount of problems potentially caused.
    //      It may still conflict, but it's properly scoped so a conflict can be more easily noticed.
    //
    // It's strongly recommended that you specify which files are exported
    // in your theme documentation. If your theme collides with files used by the site, the site takes
    // precedence and will likely break the theme in fun and exciting ways.
    "mount": [
        "some/folder"
    ],
    "data": {
        // Data mounted to the global theme object
    }
}
```

## A note on security

Although the risk is comparatively low, you should make sure you trust the theme. It can and will add files to your site, potentially without you noticing if you just push and let it run in the CI without watching the logs. It will never override your site, but since it can add JS, it can redirect to an entire site that lives in parallel with yours.

Submodules provide automatic version pinning, as long as you don't blindly force an update on the CI, which may or may not help with supply chain attack. But do be careful when testing non-core themes; they may not be safe to use, or may even be safe and later become unsafe. Using common sense is required.
