---
{
    "date": "2026-10-23",
    "data": { "started": "01.01.2026 13:00:00+02:00" }
}
---
# Datetime management

All fields in violet registered as dates are automagically converted to a UNIX timestamp (64 bit integer with the number of seconds since 1970). This means that, for example, the date stored on this page (`"date": "2026-10-23"`) is actually {{ page.date }}.

All the time-related functions therefore take integers as arguments, and all the inputs for standard frontmatter arguments are compatible with this out of the box.

If you have a non-standard datetime in `data` and wish to format it, you must use the `parseDate()` first. See [the docs for the datetime functions](/templating/functions/datetime.md#parsedatedate-pattern) for more information


## Examples

### Handling builtin fields like `page.date` and `page.last_modified`

The builtin fields are automatically parsed as ISO-8601 dates or datetimes, and converted to an int. To format these, you just call `formatDate` directly:

```
{{ formatDate(page.date, "%FT%T%z", "Etc/GMT-1") }}
```

### Handling non-standard date fields

If you store dates anywhere else and want to handle them, you need to parse them first. You can do this with the `parseDate` method.

`parseDate` does not require a format string for ISO-8601 dates and datetimes, similar to how both `page.date` and `page.last_modified` are set up. You can still supply a format if you do have a non-standard date.

Assume you have pages with:
```markdown
---
{
    "data": { "started": "01.01.2026 13:00:00+02:00" }
}
---
```

and you want to render this date. You can do this with
```
{{ formatDate(parseDate(page.data.started, "%d.%m.%Y %T%z"), "%FT%H:%M:%OS%Oz", "Europe/Oslo")}}
```

> [!tip]
>
> For `formatDate` to produce ISO datetimes in particular, you can also use the `violet.datetime.iso` constant as the argument. See [the violet global schema](/schemas/violet.md). The full string is used here for demonstration purposes.

Rendered:

> {{ formatDate(parseDate(page.data.started, "%d.%m.%Y %T%z"), "%FT%H:%M:%OS%Oz", "Europe/Oslo")}}

For info about the format strings, see [this list](https://en.cppreference.com/cpp/chrono/parse#Format_string). For documentation about `parseDate` and `formatDate`, see [the datetime function docs](/templating/functions/datetime.md)

