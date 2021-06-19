#!/bin/bash

VERSION=$1

pyinstaller --onefile main.py

mkdir -p "release/${VERSION}"
rm -rf "release/${VERSION}/prime-practice-${VERSION}"
mkdir -p "release/${VERSION}/prime-practice-${VERSION}"

cp -r \
  ../cmake-build-release-llvm/prime-practice \
  src main.py release/main.exe \
  opening_practice.bnr \
  patch.sh patch.bat \
  ../README.md \
  "release/${VERSION}/prime-practice-${VERSION}"

cd "release/${VERSION}/"

find "prime-practice-${VERSION}" -iname "__*" | xargs rm -r

zip "prime-practice-${VERSION}.zip" \
    -r "prime-practice-${VERSION}"/*

tar -czvf "prime-practice-${VERSION}.tar.gz" \
    "prime-practice-${VERSION}"/*
