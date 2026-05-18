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

