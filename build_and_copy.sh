#!/bin/bash -xe

# call build with all params; this will set some env vars we use later
source ./build.sh "$@"

# copy the built binary to the patcher dir
cp -v "${EXTERNAL_BUILD_DIR}/default_mod.dol" ../mp1/files/default.dol
cp -v "${EXTERNAL_BUILD_DIR}/default_mod.dol" ../mp1/sys/main.dol

