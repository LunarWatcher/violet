---
{
    "title": "Templating in Violet"
}
---
# Templating

Violet uses [inja](https://github.com/pantor/inja/) as its underlying template engine.

## Extra functions

Violet provides many extra functions on top of the inja builtins. See [Extra functions](/templating/functions/README.md) and its subpages.

## Objects

There are two root-level objects that are always available:

* `page`: Contains options set by the page
* `site`: Contains options set by the site

Additionally, there are two that may be available depending on the page context:

* `listing`: A special object for paginated `page_list`s that contain all the page specified by the [paginated page list](/Page lists.md#paginated-page-lists).
* `theme`: Contains options set by the theme. Only available if a theme is set, and it defines variables. See [Themes](Themes.md).


