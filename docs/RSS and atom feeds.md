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

## Example (Atom feed, no base template)

This example shows an atom feed with no base template, so everything is contained within a single file. See the comments for more info about the violet-specific functions; the atom bits are the bare minimum to get past validation

`<site root dir>/posts/feed.atom`:
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
                <email>oliviawolfie@pm.me</email>
            </author>
        </entry>
    {% endfor %}
</feed>
```

## Example (RSS)

I did not implement an RSS feed, so I'm unable to provide an example :( If you make one, consider opening a PR? :) 
