---
{
    "title": "Escaping functions"
}
---
# Escaping functions functions

{{ page.table_of_contents }}

## `htmlEscape(content)`

HTML-escapes the provided `content` string. This function is somewhat special compared to some other implementations, and also redundantly encodes `{` and `}`, as this function is used internally in violet to avoid t emplates in code blocks from being interpreted.

## `urlEncode(content[, includeReserved = true])`

URL-encodes the provided `content` string.

The `includeReserved` param determines whether or not the function should escape a set of characters that RFC 3986 calls reserved. This includes path components. This should be set to true if you're parsing what might be a full URL into another URL, which is the default.

Example:
```
True (implicit): {{ urlEncode("https://example.com") }}

False (explicit): {{ urlEncode("https://example.com", false) }}
```

> True (implicit): {{ urlEncode("https://example.com") }}
>
> False (explicit): {{ urlEncode("https://example.com", false) }}

This form of encoding is a safer default if you embed full content from somewhere in a URL, which is why `includeReserved` is true by default.
