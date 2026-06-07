---
{
    "title": "Page generation system"
}
---
# Page generation system

This page describes how violet generates sites. This is mainly useful for developing violet.

## Main loop: copying, parsing, and generation of explicitly defined files

The main loop of the generation system does two main actions:

* If the file isn't a markdown file, or is an HTML file without frontmatter, provided it isn't ignored: the file is copied straight to the output folder
* If the file is a markdown file, the file is parsed to HTML, and rendered
* If the file is an HTML file with frontmatter, the file is rendered.

This operates directly on the source directory, and iterates through the files.

Certain filetypes may break this pattern. The vast majority of pages are 1:1, meaning one input file equals one output file. Paginated page lists, however, output N + 1 pages, where N is the number of pages required to fit the content. Note that N > 0, so one page is always generated. An additional page is generated for page 1, since it generates to both `{prefix}/page/1/index.html` and `{prefix}/index.html`.

For all intents and purposes, this is the step that concerns itself with the user-generated content.

## Post-content steps

Currently none, but will come soon:tm:

