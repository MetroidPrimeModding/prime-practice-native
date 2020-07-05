#!/bin/bash -xe
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

COPY_FROM="/Users/pwootage/projects/virtualbox_folder/prime-practice-native/cmake-build-debug-remote"

cp -v "${COPY_FROM}/default_mod.dol" ../mp1/files/default.dol
cp -v "${COPY_FROM}/default_mod.dol" ../mp1/sys/main.dol
cp -v "${COPY_FROM}/Mod.rel" ../mp1/files/Mod.rel
