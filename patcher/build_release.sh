#!/bin/bash -e

VERSION=$1
if [ -z "$VERSION" ]; then
  VERSION_LINE=$(grep '#define PRAC_MOD_VERSION' src/version.h)
  VERSION=$(echo "$VERSION_LINE" | sed -E 's/#define PRAC_MOD_VERSION "([^"]+)"/\1/')
fi

echo "Building release version: ${VERSION}"

# Error if release already exists
if [ -d "release/${VERSION}/prime-practice-${VERSION}" ]; then
  echo "Release directory release/${VERSION}/prime-practice-${VERSION} already exists! Please remove it first."
  exit 1
fi

#pyinstaller --onefile main.py

mkdir "release/${VERSION}"

../build.sh Release

cp -r \
  ../cmake-build-release-docker/prime-practice \
  src main.py release/main.exe \
  pyelftools \
  opening_practice.bnr \
  patch.sh patch.bat \
  ../README.md \
  "release/${VERSION}/prime-practice-${VERSION}"

mkdir -p "release/${VERSION}/prime-practice-${VERSION}/pyelftools"
rm -rf "release/${VERSION}/prime-practice-${VERSION}/pyelftools/test"
rm -rf "release/${VERSION}/prime-practice-${VERSION}/pyelftools/examples"
rm -rf "release/${VERSION}/prime-practice-${VERSION}/pyelftools/.git"

cd "release/${VERSION}/"

find "prime-practice-${VERSION}" -iname "__pycache__" | xargs rm -r

zip "prime-practice-${VERSION}.zip" \
    -r "prime-practice-${VERSION}"/*

tar -czvf "prime-practice-${VERSION}.tar.gz" \
    "prime-practice-${VERSION}"/*
