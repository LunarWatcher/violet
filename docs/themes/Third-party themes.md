---
---
# Third-party themes

This page contains a list of third-party themes, meaning themes that are not included directly in violet.

> [!warning]
>
> The currently aren't any themes, but this page contains a placeholder list for when there are third-party themes that can be linked to.

{% set data = loadJson("third-party-themes.json", page) %}
<table>
    <thead>
        <tr>
            <th class="align-left">Repo</th>
            <th class="align-left">Oneliner</th>
            <th class="align-left">License</th>
            <th class="align-left">Author</th>
        </tr>
    </thead>
    <tbody>
        {% for row in data %}
            <tr>
                <td><a href="{{ row.url }}">{{row.name}}</a>
                {% if row.example_url != null %}(<a href="{{row.example_url}}">Demo</a>){% endif %}
                </td>
                <td>{{ htmlEscape(row.oneliner) }}</td>
                <td>{{row.license}}</td>
                <td>{{row.author}}</td>
            </tr>
        {% endfor %}
    </tbody>
</table>

## Adding a theme to this list

### Inclusion requirements

Themes must meet the following requirements to be on this list:

1. The theme must be open-source and have some open-source license.
1. The theme must have an AI ban

### Adding the theme

You'll need to make a pull request against [violet's main repo](https://codeberg.org/LunarWatcher/violet), specifically to the file [`docs/themes/third-party-themes.json`](https://codeberg.org/LunarWatcher/violet/src/branch/master/docs/themes/third-party-themes.json). Copy one of the previous rows, paste it in, and edit the details. Make sure to add a comma at the end of the now second last line so the JSON is valid.

The fields in the JSON object are:

* `name`: The name of the theme
* `oneliner`: A single, preferably short line describing the theme and its intended use. Must be plain text
* `author`: Shown to indicate who the author is. Doesn't need to be an actual name, a username works fine here too.
* `url`: URL to the source code.
* `example_url`: URL to a demo page for the theme. This doesn't have to be a dedicated demo, a link to the docs for the theme is also fine. If you don't have this, set the value to `null`
* `license`: The theme's license

## Reporting themes

Because of finite resources, the list won't be regularly checked. If you find a theme on this list that now violates one or more of the inclusion requirements, or has been removed or archived, please [open an issue](https://codeberg.org/LunarWatcher/violet/issues), or open a PR removing it from the list. Remember to include a description of the problem in both cases.

