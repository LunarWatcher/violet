---
{
    "title": "Building violet from source"
}
---
# Building violet from source

This guide is for building violet from source entirely without any of the utilities added to avoid the manual steps.

## Requirements

* CMake and make
* g++16 or newer (or possibly slightly older; must support C++23)
* Linux (possibly windows)
* Optional: Git (used for resolving the version; see [building without git context](#building-without-git-context) for an alternative)

FetchContent is used to source all other dependencies at this time.

## Steps

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/.local
make
# Optional, but strongly recommended
make test -j $(nproc)
# Actually install
make install -j $(nproc)
```

### Building without git context

If no git data is available at compile time, you'll need to manually provide the build version:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release -DVIOLET_BUILD_VERSION=v0.16.0
```

This technically does allow any arbitrary string, but for distribution, it should always be set to match violet's tagged version.

