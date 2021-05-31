#!/bin/bash

VERSION=$1

cp . .

mkdir -p "release/${VERSION}"
rm -rf "release/${VERSION}/prime-practice-${VERSION}"
mkdir -p "release/${VERSION}/prime-practice-${VERSION}"

cp -r \
  ../cmake-build-release-llvm/Mod.rel \
  symbols formats dol_patcher *.py \
  DolPatch.bin opening_practice.bnr \
  patch.sh patch.bat \
  ../README.md \
  "release/${VERSION}/prime-practice-${VERSION}"

find "release/${VERSION}/prime-practice-${VERSION}" -iname "__*" | xargs rm -r

zip "release/${VERSION}/prime-practice-${VERSION}.zip" \
    -r "release/${VERSION}/prime-practice-${VERSION}"/*

tar -czvf "release/${VERSION}/prime-practice-${VERSION}.tar.gz" \
    "release/${VERSION}/prime-practice-${VERSION}"/*
