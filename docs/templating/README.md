---
{
    "title": "Templating in Violet"
}
---
# Templating

Violet uses [inja](https://github.com/pantor/inja/) as its underlying template engine. An introduction to inja will not be provided here.

Violet provides two ways to provide templates for pages:

* [Project-local templates](Project-local templates.md)
* [Themes](Themes.md)

These two can live together: project-local templates take precedence over the active theme, and can be used to extend and existing theme. A whole site can also be implemented entirely without a theme using exclusively project-local templates.

## Extra functions

Violet provides many extra functions on top of the inja builtins. See [Extra functions](/templating/functions/README.md) and its subpages.

## Objects

There are two root-level objects that are always available:

* `page`: Contains options set by the page
* `site`: Contains options set by the site

Additionally, there are two that may be available depending on the page context:

* `listing`: A special object for paginated `page_list`s that contain all the page specified by the [paginated page list](/Page lists.md#paginated-page-lists).
* `theme`: Contains options set by the theme. Only available if a theme is set, and it defines variables. See [Themes](/Themes.md).

## Engine limitations

As currently implemented, there are a fair few things you need to keep track of yourself while writing templates.

### `{{ site.prefix }}` must be present on all absolute internal URLs in templates

This does not apply to links in markdown, it exclusively applies to links in the inja templates. 

When writing an absolute link, you must specify `{{ site.prefix }}`. Even if your deployment lies on `/`, `-l` for local development overwrites the site prefix to be the full path to the build directory, so all your local things work.

Prefixing all absolute URLs with `{{ site.prefix }}` lets `violet generate -l` builds work, and lets browsers render the page properly, and is noop if you do deploy at `/`.

**DO:** `<a href="{{ site.prefix }}/absolute/url">text</a>`
**DON'T:** `<a href="/absolute/url">text</a>` - will not link correctly when building with `violet generate -l`
