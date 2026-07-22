---
{
    "title": "Special pages"
}
---
# Special pages

Violet is intended to be host-agnostic - this means that it tries to avoid containing features that would limit use on other hosts. "Other" in this case means "not Codeberg Pages and not GitHub Pages", since those are the two hosts I deal with regularly.

## 404 pages

The name of the 404 file varies by host. Neocities uses `not_found.html`[^1], probably most other hosts (including, notably, Codeberg Pages and GitHub Pages) use `404.html`. For the name, see the docs for whereever you're deploying the site.

You can use both `404.md` and `404.html` with violet (and equivalent for `not_found.md` and `not_found.html`, and any other filenames), and the file otherwise functions like any other page. However, 404 pages SHOULD include `"hidden": true` in the frontmatter. Markdown example:

```markdown
---
{
    "hidden": true
}
---
# Page not found!
```

`hidden` decides whether or not to include the page in iterators. Not including `"hidden": true` can and will make the 404 page show up in page lists, if it happens to be placed inside one.

[^1]: For now; [there's an open issue for supporting 404.html as a fallback](https://github.com/neocities/neocities/issues/460)
