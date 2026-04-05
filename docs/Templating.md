---
---
# Templating

Violet uses [inja](https://github.com/pantor/inja/) as its underlying template engine.

## Objects

There are two available root-level objects:

* `page`: Contains options set by the page
* `site`: Contains options set by the site
* `theme`: Contains options set by the theme. Only available if a theme is set, and it defines variables. See [Themes](Themes.md).

## Functions

Inja provides a few built-in functions, and these are not documented here. Violet adds several functions on top of the builtins for the engine to be suitable for SSG use.

### `format_time(format_string, time[, timezone])`

Parameters:

* `format_string` is a [chrono formatter-compatible string](https://en.cppreference.com/w/cpp/chrono/zoned_time/formatter.html) that defines how to render the time.
* `time` is a 64 bit int, though you'll usually want to refer to a date field in the context object that has already been parsed, such as `page.date`. These are stored as parsed uint64s, so rendering a date directly is equivalent to rendering the UNIX time.
* `timezone` is an optional parameter defining what timezone the date should be rendered in. It not supplied, the date is rendered in UTC. The timezone is a string, and must exist in the timezone database: https://en.wikipedia.org/wiki/List_of_tz_database_time_zones#UTC_offset

#### Example (ISO-8601 date)

```
{{ format_time("%FT%T%z", page.date, "Etc/GMT-1") }}
```

> [!warning]
> The `Etc/GMT[offset]` timezones are filthy fucking liars because the signs are inverted. `Etc/GMT-1` is UTC+1, `Etc/GMT+1` is UTC-1. You can also convert to a location timezone if you prefer here; as long as it's in your system's timezone database, violet does not care.

Given
```
---
{ "date": "2026-03-30T20:02:51+0200" }
---
```
This would render `2026-03-30T19:02:51+0100`


