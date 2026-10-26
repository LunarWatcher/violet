#!/usr/bin/bash
export VERSION=$(git describe --tags --abbrev=0)

if [ ! -f /tmp/violet-$VERSION.tar.gz ]; then
    wget https://codeberg.org/LunarWatcher/violet/archive/$VERSION.tar.gz -O /tmp/violet-$VERSION.tar.gz
else
    echo "Note: using the already downloaded /tmp/violet-$VERSION.tar.gz"
fi

export CHECKSUM=$(sha256sum /tmp/violet-$VERSION.tar.gz | awk '{print $1}')
echo "Checksum resolved to $CHECKSUM"

cat PKGBUILD | envsubst '$VERSION,$CHECKSUM' > PKGBUILD-dist
