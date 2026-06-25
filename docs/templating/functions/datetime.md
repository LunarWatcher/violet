---
{
    "title": "Datetime function reference"
}
---
# Datetime functions

{{ page.table_of_contents }}

## `formatDate(time, format_string[, timezone])`

Parameters:

* `time` is a 64 bit int, though you'll usually want to refer to a date field in the context object that has already been parsed, such as `page.date`. These are stored as parsed uint64s, so rendering a date directly is equivalent to rendering the UNIX time.
* `format_string` is a [chrono formatter-compatible string](https://en.cppreference.com/w/cpp/chrono/zoned_time/formatter.html) that defines how to render the time.
* `timezone` is an optional parameter defining what timezone the date should be rendered in. It not supplied, the date is rendered in UTC. The timezone is a string, and must exist in the timezone database: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones#UTC_offset

### Example (ISO-8601 date)

```
{{ formatDate(page.date, "%FT%T%z", "Etc/GMT-1") }}
```

> [!warning]
> The `Etc/GMT[offset]` timezones are filthy fucking liars because the signs are inverted. `Etc/GMT-1` is UTC+1, `Etc/GMT+1` is UTC-1. You can also convert to a location timezone if you prefer here; as long as it's in your system's timezone database, violet does not care. Using named timezones is recommended. A future expansion into raw offsets is planned.

Given
```markdown
---
{ "date": "2026-03-30T20:02:51+0200" }
---
```
This would render `2026-03-30T19:02:51+0100`

## `now()`

Returns the internal representation of the time when the function is invoked. Must be passed through `formatDate` to be human-readable
