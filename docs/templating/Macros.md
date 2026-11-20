---
{
    "title": "User-defined functions (macros)"
}
---
# User-defined functions (macros)

Violet 0.21.0 and newer supports user-defined functions through `{% macro %}` blocks. The documentation for macros can be found [in iris' documentation](https://lunarwatcher.codeberg.page/iris/language/macros.html), and will not be broadly elaborated here.

For macros to be usable in violet, and for macros to be used across files, you must use the `_macros` folder. This is available in both themes and project-local templates. Note, however, that if you write a macro with the same name as a macro defined in a theme, **your local macro takes precedence** and fully overrides the theme macro. As a result, unless the override is intentional, it's recommended to always prefix your macros with a unique-ish prefix. This is purely intended as a convention and is not enforced, but you should prefix them. For example, all the macros in violet's builtin themes use the `violet` prefix (i.e. `violetTreeBuilder()`).

> [!warning]
>
> If you ignore this and write a function name that later collides with a function built into iris or violet, your function will be outright ignored regardless of where it's defined.

## When to use macros

Macros are useful for including standard template things that you use in multiple places, **and that requires input**. If you don't need any input, i.e. a zero-argument macro, a partial that you `{% include %}` is functionally identical.

If you have input, or potentially do recursive includes, you should use macros.

## Difference between local and global macros

This page primarily discusses global macros, but local macros are also an option.

* A global macro is a macro defined in the `_macros/` folder. It's available in all files without needing to include anything
* A local macro is a macro defined in the current file (or in the future, a macro that is defined in another file and is manually included - which, again, is not currently supported).

If you need to use a macro in multiple files (read: any number greater than 1), use global macros. This advice will partly change in the future, but there's effectively no extra cost to making macros global. 

## Notes

**Filenames do not matter**, and you can define multiple macros in a single file. Filenames do NOT need to match the macros defined in them, though this is recommended to make the project easier to understand.

The macro folder may have subfolders, but these serve no semantic meaning.

All macros in the macro folder are loaded globally, and must therefore have globally unique names. Local macros cannot override whatever you have defined globally.

At this time, it's not possible to `{% include %}` a file to get its macros. This will be supported in the future.

## Example usage

This is a real example from violet's default documentation theme, specifically for the macro used to build the navigation tree shown in the sidebar.

`_macros/tree-builder.iris`
```jinja
{% macro violetTreeBuilder(currTree) %}
    {# Direct root descendants #}
    {% for p in currTree.items %}
        <li><a href="{{ abspath(p.url) }}">{{ p.title }}</a></li>
    {% endfor %}

    {% for nextTree in currTree.subfolders %}
        <li>
            {% if nextTree.index != null %}
                <a href="{{ abspath(nextTree.index.url) }}">{{ nextTree.index.title }}</a>
            {% else %}
                {{ nextTree.folder_name }}
            {% endif %}
            <ul>
                {{ violetTreeBuilder(nextTree) }}
            </ul>
        </li>
    {% endfor %}
{% endmacro %}
```

This is used to build the sidebar navigation you see on the left side of violet's documentation site, with the full page tree. Specifically, the macro part is used for the recursive tree bits, while the root tree setup is partly set up in the theme's `base.iris`, since that's where the sidebar is defined.

The actual macro use is identical to any other function, as already demonstrated by the macro doing a recursive call to itself. Here the call is again in the root call:

```jinja
<aside id="_default-index">
    <h2>Index</h2>
    {% set subtree = treePages("/") %}
    {% if subtree.index != null %}
        <p><a href="{{ abspath(subtree.index.url) }}">{{ subtree.index.title }}</a></p>
    {% endif %}
    <ul>
        {{ violetTreeBuilder(subtree) }}
    </ul>
</aside>
```
