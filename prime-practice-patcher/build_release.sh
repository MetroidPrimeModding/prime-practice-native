#!/bin/bash

VERSION=$1

cp . .

mkdir -p "release/${VERSION}"
rm -rf "release/${VERSION}/prime-practice-${VERSION}"
mkdir -p "release/${VERSION}/prime-practice-${VERSION}"

cp -r \
  ../cmake-build-release-llvm/Mod.rel \
  symbols \
  src \
  DolPatch.bin opening_practice.bnr \
  patch.sh patch.bat \
  ../README.md \
  "release/${VERSION}/prime-practice-${VERSION}"

find "release/${VERSION}/prime-practice-${VERSION}" -iname "__*" | xargs rm -r


cd "release/${VERSION}/"
zip "prime-practice-${VERSION}.zip" \
    -r "prime-practice-${VERSION}"/*

tar -czvf "prime-practice-${VERSION}.tar.gz" \
    "prime-practice-${VERSION}"/*
