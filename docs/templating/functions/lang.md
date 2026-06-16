---
{
    "title": "Language functions"
}
---
# Language functions

This category of functions describes extension functions to the inja language, that arguably should be implemented but aren't.

{{ page.table_of_contents }}

## `recurseInclude(templateName, context)`

TODO: rename to `includeContext`

This function can be used to include a template and provide the context object directly. Example usage:
```jinja
{# For whatever reason, setting the context inline does not work and results in a syntax error. #}
{# This appears to maybe be a limitation of inja, but I have not verified #}
{% set context = {} %}
{% set context.currTree = subtree %}
{% set context.page = page %}
{% set context.site = site %}
{{ recurseInclude("partials/tree_builder.inja", context) }}
```

In this case, we also have to manually build the context to include `page` and `site`. Otherwise, `page` and `site` are excluded outright.

This is primarily required to properly context scope certain objects for recursion. Trying recursive includes of templates will otherwise result in a segfault from the underlying JSON library.

### Arguments

#### `templateName`

Self explanatory; same as what you would pass to a regular `{% include "templateName" %}`

#### `context`

A single object. All types are allowed, and what you pass depends on what you need in the templates you include.

## `createPaginatedList(pageNum, totalPages)`

Utility method for creating paginated lists. Due to some limitations in inja, it appears to be much harder to create pagination widgets entirely in inja without resorting to deeply cursed hacks.

Example use:
```inja
<ul>
    {% for i in createPaginatedList(listing.page, listing.total_pages) %}
        <li title="Go to page {{ i }}">
            <a href="{{ site.prefix }}/{{ paginatedUrl(listing.base_path, i) }}"
                id="next-page"
                aria-label="Next page"
                {% if listing.page == i %}class="current"{% endif %}
            >{{ i }}</a>
        </li>
    {% endfor %}
</ul>
```

### Arguments

* `pageNum`: the current page. Usually `listing.page`
* `totalPages`: The total number of pages. Usually `listing.total_pages`

### Return value

A list of integer indices that can be used to construct a page. Assuming enough pages, the list returned is:
```
[
    1, 2, 3,
    listing.page - 1, listing.page, listing.page + 1,
    listing.total_pages - 2, listing.total_pages - 1, listing.total_pages
]
```

deduplicated and filtered for legal values. The max numbers of indices returned by this method is 9.

