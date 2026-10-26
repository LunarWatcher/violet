# Packaging notes

For build instructions, see [docs/build.md](docs/build.md).

## `PKGBUILD`

`PKGBUILD` is not a "finished" PKGBUILD, and is not maintained as such. It's used as a template to generate the actual `PKGBUILD`, because I hate maintaining versioned things that could be scripted away.

To generate an actual `PKGBUILD` for release, use `./scripts/pkgbuild-gen.sh`.

For testing:
```bash
./scripts/pkgbuild-gen.sh
BUILDDIR=/tmp/violet-pkgbuild makepkg -p $(pwd)/PKGBUILD-dist
```

Please note that this always fetches the latest release from codeberg (assuming the local repo is up to date).

> [!warning]
>
> Do not run this without specifying `BUILDDIR` if you have uncommitted changes in the `src` folder. If you don't pass `BUILDDIR`, the entire archive is unzipped within `src` in a way that collides with at least a couple files. The easiest way to recover from this is to remove the entire `src` folder and `git checkout HEAD -- src` it, but this purges any actual changes you have.

