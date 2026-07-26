---
{
    "title": "Schema for _violet-meta.json"
}
---
# Schema for `_violet-meta.json`

Note: `_violet-meta.json` is disabled by default. Set `"metadata": true` in `violet.json` to enable it. See [the config spec](/Config.md) for more info.

The `_violet-meta.json` file is meant to be loaded with javascript by the page to populate stats. You should never use `loadJson` on this file, as it will not exist on a first build, thereby crashing the build, and when it does exist, it will always use the last build. For obvious reasons, violet cannot know how much stuff there is to process until the stuff has been processed, so a page trying to contain such information must use a separately generated file, or there's a chicken and the egg problem with the generation.

The schema may change, but removals are actively avoided to this schema in particular to maintain backwards compatibility. New fields may be added at any time, so if you run some weird JSON loading system that requires a hard-coded schema to be able to load the file at all, it will likely start failing at some point.

```json
{
    // Defines numbers related to the input files. This always describes the raw number of input files, and not their output.
    // A paginated page with something absurd like 1000 pages in its list will only count as 1 page here
    // The total number of input files in the project's filesystem is described by input.pages + input.files.
    "input": {
        // The number of pages, meaning .html (with frontmatter) and .md files with template processing, as well as .css, .js, and .atom/.xml files rendered with inja.
        // 
        "pages": 1234,
        // The number of non-page files, meaning .html (without frontmatter), and any other files not rendered with inja
        // This means anything counted by `pages` is excluded from `files`.
        "files": 1234
    },
    // Defines numbers related to the output files. This describes the number of files violet output into the build
    // directory. Please note that "files" is missing here, because input.files == output.files, while
    // output.pages >= input.pages due to paginated pages.
    "output": {
        // Defines the number of pages rendered through inja.
        "pages": 1234
    },
    "build": {
        // The number of milliseconds the build took.
        "time_ms": 1234,
        // The time the build was started at, represented as milliseconds since the UNIX epoch in UTC+0.
        "started_at": 12345678,
        // The version of violet the page was built with.
        // Debug versions differ from the major.minor.patch version system, and have an extra hash appended - please be aware of this if you try parsing the versions.
        // The exact format depends on the output of `git describe --tags --always`, but can look like
        // v0.6.0-2-g66477f8
        "version": "0.7.0"
    }
}
```

## Current limitations

* Assets are considered files in spite of being template-processed. [#14](https://codeberg.org/LunarWatcher/violet/issues/14) will necessitate changes here, and likely add another category. Not doing that for now because I'm not entirely sure how I want to go about it.
