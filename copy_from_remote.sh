#!/bin/bash -xe
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

COPY_FROM="pwootage@192.168.50.6:/home/pwootage/remote-cmake/tmp/tmp.eHZf1dbHIe/cmake-build-debug-devkitpro-remote"

#scp "${COPY_FROM}/default_mod.dol" ../mp1/files/default.dol
#cp ../mp1/files/default.dol ../mp1/sys/main.dol
scp "${COPY_FROM}/Mod.rel" ../mp1/files/Mod.rel
