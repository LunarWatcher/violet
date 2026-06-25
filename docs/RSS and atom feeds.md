---
{
    "title": "RSS and atom feeds"
}
---
# RSS and Atom feeds

Unlike in most static site generators, violet does not _directly_ come with facilities for RSS and atom feeds linked to other features. Instead, you have to manually define and declare RSS and/or Atom feeds yourself.

One of the biggest annoyances I've had with other static site generators is that the feeds just magically disappear. You can maybe get a URL from it that you can put into the frontmatter, but this can change if/when the static site generator changes. Instead, violet treats RSS and Atom feeds (or more broadly, files with the extensions `.rss`, `.atom`, and `.xml` with frontmatter) as a special type of file. This means you know precisely where the feeds on your site are, because you've declared them.

## Lookup locations

RSS and Atom feeds get a bit of special treatment since it's a whole other content type.

For the most part, the logic is the same as for HTML and markdown files, which is described in [the lookup order docs](/Project structure.md#lookup-order). However, they have two differences:

1. If no `layout` is defined, `feed` is used instead of `single_page`
2. If no matching layouts exist, the feed is parsed standalone. In other words, feeds are allowed to have no template base.

## Resources

* Atom feed reference: [https://validator.w3.org/feed/docs/atom.html](https://validator.w3.org/feed/docs/atom.html)
  * WARNING: The sample feed lies. It uses `url:uuid`, which is invalid, but a later link buried on this page ([this one](https://web.archive.org/web/20110915110202/http://diveintomark.org/archives/2004/05/28/howto-atom-id#nourn)) describes why url links shouldn't be used. This is a huge oversight by the w3c. TL;DR: use links, not UUIDs. Violet cannot generate stable UUIDs anyway, so links are much more stable.
* Atom validator: [https://validator.w3.org/feed/check.cgi](https://validator.w3.org/feed/check.cgi)

## Examples

### Linking to a feed

Links work exactly the way you'd expect; if you have `blog/feed.atom`, you'll find it at `/blog/feed.atom` on the rendered site.

If you want to link to your feeds in your `<head>`, you'd do it like you'd load a CSS or JS file, though I prefer using 
```
<link rel="alternate"
   title="Olivia's silly blog :3 - blog posts"
   type="application/atom+xml"
   href="{{ site.data.domain }}/posts/feed.atom" />
```

### Atom feed with no base template

This example shows an atom feed with no base template, so everything is contained within a single file. See the comments for more info about the violet-specific functions; the atom bits are the bare minimum to get past validation

`<site root dir>/blog/feed.atom`:
```inja
---
---
<!-- ^ All the feeds require a frontmatter to render. It does not need to contain anything if you don't do advanced templating -->
<?xml version="1.0" encoding="utf-8"?>
<feed xmlns="http://www.w3.org/2005/Atom">
    <title>{{ site.name }} - blog posts</title>
    <link href="https://lunarwatcher.codeberg.page/blog/feed.atom" rel="self" />
    <link href="https://lunarwatcher.codeberg.page/blog/" />
    <id>https://lunarwatcher.codeberg.page/</id>
    <updated>{{ formatDate(now(), violet.datetime.iso) }}</updated>

    <!-- Since feed.atom is alongside the pages it wants to list, it uses the object page and the scope . (the . is
    relative to the page's path, which is stored through internal magic in the page object that isn't important)
    This function therefore iterates /posts/*, and sorts by last_modified. The other sort method currently implemented
    is violet.sort.by_creation_date. I chose by_last_modified_date to allow this feed to bump pages I edit.

    lastPagesPaginated differs from listPages in the number of pages returned. The three-argument form
    sets pagesize to 50 and page to 1. For an RSS feed, page should in practice always be 1, and it can be omitted.
    See /templating/functions/files.html (File operation functions) for more info on the function 
    -->
    {% for entry in listPagesPaginated(page, ".", violet.sort.by_last_modified_date) %}
        <entry>
            <!-- All userdata must be escaped. Here, I use CDATA, because it's the easiest option. You can also use
                {{ htmlEscape(entry.title) }}
            though it's far more aggressive. The formatting is the same in XML and HTML, but HTML requires more escapes
            than XML does.
            -->
            <title><![CDATA[{{ entry.title }}]]></title>
            <!-- At this time, violet doesn't have a way to reference the domain. {{ site.prefix }} is just relative to the domain
            to allow for easy deployments to both github pages and codeberg pages from the same generated source. 
            You can use {{ site.data }} if you want to get around this for your site. Better design is clearly needed, and input
            is very much welcome here :) -->
            <link rel="alternate" href="https://lunarwatcher.codeberg.page/{{ entry.url }}" />
            <id>https://lunarwatcher.codeberg.page/{{ entry.url }}</id>

            {% if existsIn(entry, "date") %}
                <published>{{ formatDate(entry.date, violet.datetime.iso) }}</published>
            {% else %}
                <published>{{ formatDate(now(), violet.datetime.iso) }}</published>
            {% endif %}
            {% if existsIn(entry, "last_modified") %}
                <updated>{{ formatDate(entry.last_modified, violet.datetime.iso) }}</updated>
            {% else if existsIn(entry, "date") %}
                <updated>{{ formatDate(entry.date, violet.datetime.iso) }}</updated>
            {% else %}
                <updated>{{ formatDate(now(), violet.datetime.iso) }}</updated>
            {% endif %}
            <!-- Violet currently has no way to provide <content> at this time, so summary is the closest we get.
            This has so far been avoided to avoid unnecessarily loading entire files when metadata is all that matters.
            This will be patched in the future, though is not currently prioritized. Feel free to make a PR :3 -->
            {% if existsIn(entry, "summary") %}
                <summary><![CDATA[{{ entry.summary }}]]></summary>
            {% endif %}
            <!-- My reference site only has one author. You can use {{ entry.data }} if you have a multi-author site. -->
            <author>
                <name>Olivia</name>
                <email>email@example.com</email>
            </author>
        </entry>
    {% endfor %}
</feed>
```

### Atom feed using template

This example is the same in principle as the standalone example. Its comments apply here as well, and have not been duplicated. However, it relies far more heavily on `{{ page.data }}`, and rather than copying it into its final destination, we put it in `_templates/_default/feed.inja`:
```inja
<?xml version="1.0" encoding="utf-8"?>
<feed xmlns="http://www.w3.org/2005/Atom">
    <title>{{ site.name }} - {{ page.data.name }}</title>
    <link href="{{ site.data.domain }}/{{ page.url }}" rel="self" />
    <!-- Technically, you can have https://example.com/feed.atom that provides a feed for https://example.com/blog.
    Also, violet does not yet provide anything that lets you strip components from a URL, so this is a cheap workaround
    -->
    <link href="{{ site.data.domain }}{{ page.data.for-scope }}" />
    <id>{{ site.data.domain }}/{{ page.url }}</id>
    <updated>{{ formatDate(now(), violet.datetime.iso) }}</updated>

    {% for entry in listPagesPaginated(page, ".", violet.sort.by_last_modified_date) %}
        <entry>
            <title><![CDATA[{{ entry.title }}]]></title>
            <link rel="alternate" href="{{ site.data.domain }}/{{ entry.url }}" />
            <id>{{ site.data.domain }}/{{ entry.url }}</id>

            {% if existsIn(entry, "date") %}
                <published>{{ formatDate(entry.date, violet.datetime.iso) }}</published>
            {% else %}
                <published>{{ formatDate(now(), violet.datetime.iso) }}</published>
            {% endif %}
            {% if existsIn(entry, "last_modified") %}
                <updated>{{ formatDate(entry.last_modified, violet.datetime.iso) }}</updated>
            {% else if existsIn(entry, "date") %}
                <updated>{{ formatDate(entry.date, violet.datetime.iso) }}</updated>
            {% else %}
                <updated>{{ formatDate(now(), violet.datetime.iso) }}</updated>
            {% endif %}
            {% if existsIn(entry, "summary") %}
                <summary><![CDATA[{{ entry.summary }}]]></summary>
            {% endif %}
            <author>
                <name>Olivia</name>
                <email>email@example.com</email>
            </author>
        </entry>
    {% endfor %}
</feed>
```

Now, `/blog/feed.atom` becomes the much shorter:
```xml
---
{
    "data": {
        "name": "blog posts",
        "for-scope": "/posts/"
    }
}
---
```

... no, really, that's it. Now you can make multiple feeds. Due to the use of the "." scope, they'll all generate relative to the location of the final `feed.atom`. Unfortunately, you now can't make RSS feeds

### RSS feed

I did not implement an RSS feed, so I'm unable to provide an example :( If you make one, consider opening a PR? :)

### Combined feed

At this time, the only way to create a combined feed is to repeat the same process as for a single feed multiple times. Atom feeds [fortunately do not require a specific order](https://datatracker.ietf.org/doc/html/rfc4287#section-4.1.1), so this is technically legal, but not optimal. Doing it with multiple manual entries does mean that the feed _will_ always contain at least N elements from each feed, though, so it's not all bad.

Here's an abridged example based on the Atom feed example, using the file `/combined.atom`:
```inja
<?xml version="1.0" encoding="utf-8"?>
<feed xmlns="http://www.w3.org/2005/Atom">
    <title>{{ site.name }} - {{ page.data.name }}</title>
    <link href="https://example.com/combined.atom" rel="self" />
    <link href="https://example.com" />
    <id>{{ site.data.domain }}/combined.atom</id>
    <updated>{{ formatDate(now(), violet.datetime.iso) }}</updated>

    <!-- The second argument is now used to iterate subfolders of the root rather than the entire root
    You can in theory iterate the root as well, but this really depends on your site and its needs.
    For my use, I avoid iterating the root since I have non-blog content I don't want in the feed.
    -->
    {% for entry in listPagesPaginated(site, "blog", violet.sort.by_last_modified_date, 10) %}
        <entry>
            <!-- Entry content omitted for brevity -->
        </entry>
    {% endfor %}

    {% for entry in listPagesPaginated(site, "til", violet.sort.by_last_modified_date, 10) %}
        <entry>
            <!-- Entry content omitted for brevity -->
        </entry>
    {% endfor %}

    {% for entry in listPagesPaginated(site, "notes", violet.sort.by_last_modified_date, 10) %}
        <entry>
            <!-- Entry content omitted for brevity -->
        </entry>
    {% endfor %}
</feed>
```

The extra parameter to `listPagesPaginated` (the `10`) is the page size. We reduce it to 10 here purely to avoid getting up to 150 elements in the final combined feed. How you go about this is up to you.

