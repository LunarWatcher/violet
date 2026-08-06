---
{
    "title": "Templating non-supported filetypes"
}
---
# Templating non-supported filetypes

Supported filetypes are files with one of the following extensions:

* `.md`, `.html`: standard content files, defaults to `"layout": "single_page"`
* `.js`, `.mjs`, `.css`: asset files, defaults to `"layout": "none"`
* `.xml`, `.rss`, `.atom`: feed files, defaults to `"layout": "feed"`. Note that you can have `.xml` files that aren't feeds, as there are no URL translations, this is just the default assumption.

If you need to load any other file with templating support, for example to generate other files based on `.json` files, you can use the special `.in` filetype. For example, if you want to create a template in a file called `sitemap.json` (not a standard thing, just an example) with content based on the pages on the site, you can call the file `sitemap.json.in`. When compiled, it's converted to the name `sitemap.json` - the `.in` extension is always stripped. However, it now has template semantics. The default for these is to use `"layout": "none"`, but you can use layouts for these as well if you can think of a use-case for it.

```json
[
    {% for page in listPages(site, ".") %}
        {
            "title": "{{ page.title }}",
            "url": "{{ site.prefix }}/{{ page.url }}"
        }{% if not loop.is_last %},{% endif %}
    {% endfor %}
]
```

> [!warning]
>
> As with all other filetypes, beware of accidental input injection. If a title has quotes, it'll break the resulting JSON. 
>
> There is currently no way to escape such content. HTML and HTML-shaped things have the `htmlEscape` method, and XML in particular lets you use CData that gets around more of it, but violet currently lacks a general/customizable escaping function. Such a function is planned for 0.9.0.

