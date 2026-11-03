---
{
    "title": "Previewing a site"
}
---
# Previewing a site

Violet offers two (three) main ways to preview a site while working on it:

1. By using `violet generate`:
  1. With `-l`: for viewing raw files in any browser
  2. With `-p /`: For use with [firefox local mode](https://firefox-source-docs.mozilla.org/devtools-user/local_mode/index.html)
2. `violet serve`

There are no major differences between these two (three) aside maybe preference. They all let you see the site, and they all have ways to continuously regenerate the page.

## Using `violet serve`

> [!warning]
>
> `violet serve` is added in 0.19.0, which is not yet released.

`violet serve` spins up a local server that you can use for preview. Similar to `violet generate -w`, it watches for changes in the source folder to continuously rebuild the site. `violet serve` does NOT inject anything into the page at present, nor is it able to signal that a rebuild has happened, so you'll still need to refresh your browser manually to see changes.

Note that the `serve` command is linux-only, as the underlying web framework does not currently support windows.

Workflow:

1. `violet serve`
2. Go to the localhost address printed in your console

## Using `violet generate`

### Using the file local mode flag

`-l`/`--local` flag can be used to replace the prefix with a file path. This allows you to open the file in a browser as if it was a regular file. The main disadvantage with this is that any `fetch` requests you may have against the local site, for example to the meta file, will not work.

However, this works in all browsers that can open an HTML file.

Workflow:

1. `violet generate -l`
2. `xdg-open pages/index.html` and navigate from there

---

As an aside, I do strongly recommend using this approach. I do really like firefox local mode myself, but it also lets you see the filesystem. `/` -> `/index.html` does not exist in "file browsing mode" or whatever in firefox, which means you can navigate the output folder in your browser. I have found this useful at times.

### Using Firefox local mode

[Firefox local mode](https://firefox-source-docs.mozilla.org/devtools-user/local_mode/index.html) lets you navigate to a folder as if it were a website in firefox. For actually using Firefox local mode, see Firefox's documentation.

However, for this to work reliably, you need to pass the extra `-p /` flag to `violet generate`. This overrides the prefix while generating, and this is a requirement for this to work properly.

Workflow:

1. `violet generate -p /`
2. Open site in firefox local mode and navigate from there

