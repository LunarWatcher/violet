# violet

A bare minimum static site generator

## Why?

All the good SSGs have turned to AI slop, and migrating between SSGs is a pain in the ass, so I'm rolling my own that'll remain slop-free forever.

## Requirements

* C++23

### Additional requirements for serving

> [!info]
> Serving is months out. Generating takes higher priority, and this is already a tangent on top of a tangent, so this project is fairly low priority.

Violet has a built-in server for local previews based on [magpie](https://codeberg.org/LunarWatcher/magpie). The serve command is disabled by default to allow windows builds, and to allow violet to me embedded in other servers. If enabled, magpie's dependencies are added to the list.

Due to limitations of the underlying socket library at the time of writing, magpie and by extension the `serve` command are Linux-only, and therefore require linux to work.

