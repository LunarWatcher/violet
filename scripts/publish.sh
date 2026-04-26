#!/usr/bin/bash

git_root=$(git rev-parse --show-toplevel)
cd "$git_root"
if [[ -f build/bin/violet ]]; then
    violet_exe="$(pwd)/build/bin/violet"
else
    echo "Warning: didn't find locally built version of violet. Using PATH instead"
    violet_exe=violet
fi

cd docs
rm -rf pages

time "$violet_exe" generate

if [[ $? != 0 ]]; then
    echo "Generation failed"
    exit 1
fi

cd pages
git init
git checkout -b pages
git remote add cb git@codeberg.org:LunarWatcher/violet

git add -A
git commit -m "Generate page"

git push -fu cb pages
