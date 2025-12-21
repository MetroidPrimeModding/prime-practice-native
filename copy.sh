#!/bin/bash -xe
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

COPY_FROM="${DIR}/cmake-build-debug-docker-prime-build"

cp -v "${COPY_FROM}/default_mod.dol" ../mp1/files/default.dol
cp -v "${COPY_FROM}/default_mod.dol" ../mp1/sys/main.dol
