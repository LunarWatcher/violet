---
{
    "title": "Templating non-supported filetypes"
}
---
# Templating non-supported filetypes

If you need to load any [unsupported filetype](/templating/README.md#filetypes-and-default-layouts) with templating support, for example to generate other files based on `.json` files, you can use the special `.in` filetype. For example, if you want to create a template in a file called `sitemap.json` (not a standard thing, just an example) with content based on the pages on the site, you can call the file `sitemap.json.in`. When compiled, it's converted to the name `sitemap.json` - the `.in` extension is always stripped. However, it now has template semantics. The default for these is to use `"layout": "none"`, but you can use layouts for these as well if you can think of a use-case for it.

```json
[
    {% for page in listPages(site, ".") %}
        {
            "title": "{{ replace(page.title, "\"", "\\\"" }}",
            "url": "{{ site.prefix }}/{{ page.url }}"
        }{% if not loop.is_last %},{% endif %}
    {% endfor %}
]
```

> [!warning]
>
> As with all other filetypes, beware of accidental input injection. In this case, it's avoided with the inja-builtin `replace` function. For HTML, use `htmlEscape`. For XML, use `htmlEscape` in combination with CData. See also [the full list of special-purpose escaping functions](/templating/functions/escaping.md).

This is then processed to a JSON file with the title and relative URL of every page on the site, as you would expect. These also support frontmatter, but it is not required.

## URL translation for non-supported filetypes

Non-supported filetypes do not have URL translation, beyond stripping `.in` from the path. This means that if you create an invalidly named file, it will have that same invalid name in the output. In the future, these may be subject to the same URL normalization methods.

## Limitations in iteration

At this time, `.in` files are fully omitted from `listPages`. A separate function is planned but not scheduled, as I do not see any immediate need to iterate over such files. If you have such a need, please open an issue.
