---
{
    "title": "The violet global variable"
}
---
# The `violet` global variable

> [!note]
> This schema refers to the `violet` variable available in inja templates.

The `violet` global is a utility variable with constants and certain function inputs. It must NOT be modified. This is not currently enforced by violet, but will be as soon as I bother figuring out how to do it (if it's even possible to enforce; we'll see).

{{ page.table_of_contents }}

## `violet.datetime`

`violet.datetime` is a utility scope. Its use is optional unless otherwise noted by subcategories.

### `violet.datetime.iso` (string constant)

Shorthand for the ISO 8601 format string (`"%FT%H:%M:%OS%Oz"`), for use with `formatDate()` and the likes.

## `violet.sort` (enum constants)

`violet.sort` contains keys referring to sorting methods. Its use is mandatory with functions that require sorting. Sorting is currently implemented entirely in C++ code, and these keys refer to which sorting method is used.

In the future, I would like to replace this with more dynamic sorting, but this is hard to do without sourcing tons of data into inja, which will tank performance.

### `violet.sort.by_creation_date` (enum constant value)

Refers to the sort method that sorts by (in order):

1. `created`, if present
3. The filename (fallback)

### `violet.sort.by_last_modified_date` (enum constant value)

Refers to the sort method that sorts by (in order):

1. `last_modified`, if present
2. `created`, if present
3. The filename (fallback)

