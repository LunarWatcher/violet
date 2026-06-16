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

## Building for deployment

Deployment builds are done without the `-l` flag, so just `violet generate`. Note, however, that violet does not manually clean out the build directory before use, so if you delete anything, it'll stay in the output directory.

### Manual deployments

The recommended way to deploy manually is to use a script like this:
```bash
#!/usr/bin/bash

# Optional, largely exists to make it easier to move to debug builds
VIOLET_EXE=${VIOLET_EXE:-violet}

echo "Using $VIOLET_EXE"

git_root=$(git rev-parse --show-toplevel)
cd "$git_root"

rm -rf pages
time "$VIOLET_EXE" generate

if [[ $? != 0 ]]; then
    echo "Generation failed!"
    exit 1
fi

cd pages
git init
git checkout -b pages
git remote add cb git@codeberg.org:LunarWatcher/pages
# Optional: extra mirrors
# git remote add gh git@github.com:LunarWatcher/lunarwatcher.github.io

git add -A
git commit -m "Generate page"

git push -f cb pages
# git push -f gh pages
```

### Automatic deployments

#### Codeberg CI

Direct support for deployment via Codeberg is planned, but not scheduled. Codeberg's limited CI resources make me hesitant to just test and see what works and what doesn't.

If you deploy a violet site with Codeberg's CI, consider opening a PR and describing how here :)

#### GitHub Actions

```yaml
name: Build docs

on:
  push:
    branches:
      - master

permissions:
  contents: read
  pages: write
  id-token: write

concurrency:
  group: github-pages
  cancel-in-progress: false

jobs:
  deploy:
    # Violet requires a version of GCC not yet offered by ubuntu 24 (which is ubuntu-latest),
    # so we need to use ubuntu:latest, which paradoxically is newer than
    # ubuntu-latest. 
    # GitHub Actions, brought to you by Microslop
    runs-on: ubuntu-24.04
    container: ubuntu:latest
    environment:
      name: github-pages
      url: ${{ steps.deployment.outputs.page_url }}
    steps:
      - name: Install deps
        run: |
          apt update && apt upgrade -y
          apt install -y gcc build-essential make git libssl-dev libcurl4-openssl-dev
      - uses: actions/checkout@main
      - name: Fix git ownership
        run: |
          git config --global --add safe.directory $(pwd)
      - uses: LunarWatcher/install-violet@master
      - name: "Build page"
        run: |
          cd docs
          violet generate
      - uses: actions/configure-pages@v6
      - uses: actions/upload-pages-artifact@v5
        with:
          path: 'docs/pages'
      - uses: actions/deploy-pages@v5
        id: deployment
```


## Next steps

Depending on what your site is for and how hard you want to go into theming, you can:

* Write content. The default theme is intended for documentation use, and contains all the basic content types. See also [the docs for the theme](/themes/Default.md) for information about the extra config options it provides.
* Write a reusable theme for your site and other sites. See [Themes](/Themes.md) and [the templating intro](/templating/README.md)
* Write a project-specific theme for your site to use. See [the templating intro](/templating/README.md)



