---
{
    "title": "Datetime function reference"
}
---
# Datetime functions

{{ page.table_of_contents }}

## `formatDate(time, format_string[, timezone])`

Parameters:

* `time` is a 64 bit int, though you'll usually want to refer to a date field in the context object that has already been parsed, such as `page.date`. These are stored as parsed int64s, so rendering a date directly is equivalent to rendering the UNIX time.
* `format_string` is a [chrono formatter-compatible string](https://en.cppreference.com/w/cpp/chrono/zoned_time/formatter.html) that defines how to render the time.
* `timezone` is an optional parameter defining what timezone the date should be rendered in. It not supplied, `site.timezone` is used. The timezone is a string, and must exist in the system's timezone database: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones#UTC_offset

### Example (ISO-8601 date)

```
{{ formatDate(page.date, "%FT%H:%M:%OS%Oz", "Etc/GMT-1") }}
```

> [!warning]
> The `Etc/GMT[offset]` timezones are filthy fucking liars because the signs are inverted. `Etc/GMT-1` is UTC+1, `Etc/GMT+1` is UTC-1. You can also convert to a location timezone if you prefer here; as long as it's in your system's timezone database, violet does not care. Using named timezones is recommended. A future expansion into raw offsets is planned.

Given
```markdown
---
{ "date": "2026-03-30T20:02:51+0200" }
---
```
This would render `2026-03-30T19:02:51+01:00`

## `parseDate(date[, pattern])`

This function is used to parse a date stored as a string to a UNIX epoch format. This is ONLY necessary if you use a non-standard data field to store dates, or for example hard-code dates on your page. `page.date` and `page.last_modified` do NOT need to be run through this method, as they're already ints; use them directly with `formatDate`

Arguments:

* date: a string representing the date or datetime to parse
* pattern: Optional string that defines the pattern to use. Must be a [chrono parse-compatible string](https://en.cppreference.com/cpp/chrono/parse#Format_string). Note: if your date is an [ISO-8601](https://en.wikipedia.org/wiki/ISO_8601) date, you can omit this argument. If no pattern is specified, a pattern for an ISO-8601 date or datetime is automatically attempted identified.


> [!warning]
>
> You currently cannot parse just a timestamp. There MUST be a date for the call to be valid. This is currently not planned to change, as timestamps without a date are notoriously difficult to handle meaningfully, especially in C++.

## `now()`

Returns the internal representation of the time when the function is invoked. Must be passed through `formatDate` to be human-readable
