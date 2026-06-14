---
{
    "title": "Getting started"
}
---
# Getting started

{{ page.table_of_contents }}

## Installation

### From source
The recommended installation method at this time is from source

```bash
git clone https://codeberg.org/LunarWatcher/violet
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$HOME/.local/
make -j $(nproc)

# Optional: make sure everything is good first
make -j $(nproc) test

# Install
make install

# Assuming you have $HOME/.local/bin in your PATH, you should now be able to run violet
```

### From GitHub

On GitHub releases, you can find [precompiled binaries for x86_64 Linux](https://github.com/LunarWatcher/violet/releases). These are primarily intended for use with GitHub Actions. In the future, binaries meant for local end-user use will be uploaded to Codeberg.

## Setting up a project

First, you'll need a `violet.json`:

```json
{
    "name": "My awesOwOme site",
    "description": ":3"
}
```

Then you'll need a markdown file. Frontmatter in markdown files is optional, but here's an example file you can use for now (call it `README.md`):
```markdown
---
{
    "title": "hello world!",
    "summary": "This page does not contain nuclear codes."
}
---
Hewwo :3
```

With this set up, you can run `violet generate -l` to generate the HTML. Then you can open the generated with, for example, `xdg-open ./pages/index.html`. This should open it in your browser, where it should render with the default theme. Congrats, you now have an initial site setup with violet!

## Next steps

Depending on what your site is for and how hard you want to go into theming, you can:

* Write content. The default theme is intended for documentation use, and contains all the basic content types. See also [the docs for the theme](/themes/Default.md) for information about the extra config options it provides.
* Write a reusable theme for your site and other sites. See [Themes](/Themes.md) and [the templating intro](/templating/README.md)
* Write a project-specific theme for your site to use. See [the templating intro](/templating/README.md)



