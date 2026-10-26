#!/usr/bin/env bash

if ! command -v cmake; then
    echo "CMake isn't installed!"
    exit 1
fi

cd /tmp
if [ ! -d violet ]; then
    echo "Cloning violet..."
    if ! git clone https://codeberg.org/LunarWatcher/violet; then
        echo "Failed to clone violet"
        exit 1
    fi
    cd violet
else
    echo "Found /tmp/violet; trying an update"
    cd violet
    git checkout master
    if ! git pull; then
        echo "Failed to update!"
        exit 1
    fi
fi

TARGET_VERSION=$(git describe --tags --abbrev=0)

if [[ "$VIOLET_NIGHTLY" == "" ]]; then
    echo "Building $TARGET_VERSION"
    if ! git checkout "$TARGET_VERSION"; then
        echo "Failed to check out latest version"
        exit 1
    fi
else
    echo "Building nightly version"
fi

mkdir -p build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=~/.local

if ! make -j $(nproc); then
   echo "Failed to build violet!"
   exit 1
fi
if ! make -j $(nproc) test; then
    echo "Tests failed! This may be a bug you should report: https://codeberg.org/LunarWatcher/violet"
    exit 1
fi

if ! make -j $(nproc) install; then
    echo "Installation failed!"
    exit 1
fi
      

echo "Installation successful!"
echo
echo "If you haven't already, it's strongly recommended to read the getting started guide:"
echo "https://lunarwatcher.codeberg.page/violet/getting-started.html"

