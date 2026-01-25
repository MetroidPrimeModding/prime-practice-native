#!/bin/bash -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

VERSION=$1
if [ -z "$VERSION" ]; then
  VERSION_LINE=$(grep 'version' "${DIR}/src/patcher_config.toml")
  VERSION=$(echo "$VERSION_LINE" | sed -E 's/version = "([^"]+)"/\1/')
fi

echo "Building release version: ${VERSION}"

# Error if release already exists
if [ -d "release/${VERSION}" ]; then
  echo "Release directory release/${VERSION} already exists! Please remove it first."
  exit 1
fi

#pyinstaller --onefile main.py

./build.sh Release

cd release
mkdir "${VERSION}"
cd "${VERSION}"

# Windows release
mkdir "prime-practice-${VERSION}-windows/"
cp "${DIR}/cmake-build-release-docker/prime-practice" \
  "prime-practice-${VERSION}-windows/mod.elf"

cp "${DIR}/README.md" \
  "prime-practice-${VERSION}-windows/"

cp "${DIR}/release/gcn-static-patcher-windows-latest/gcn-static-patcher-gui.exe" \
  "prime-practice-${VERSION}-windows/patcher.exe"

zip "prime-practice-${VERSION}-windows.zip" \
    -r "prime-practice-${VERSION}-windows"/*

# Linux release
mkdir "prime-practice-${VERSION}-linux/"
cp "${DIR}/cmake-build-release-docker/prime-practice" \
  "prime-practice-${VERSION}-linux/mod.elf"

cp "${DIR}/README.md" \
  "prime-practice-${VERSION}-linux/"

cp "${DIR}/release/gcn-static-patcher-ubuntu-24.04/gcn-static-patcher-gui" \
  "prime-practice-${VERSION}-linux/patcher"

tar -czvf "prime-practice-${VERSION}-linux.tar.gz" \
    "prime-practice-${VERSION}-linux"/*

# Linux ARM release
mkdir "prime-practice-${VERSION}-linux-arm/"
cp "${DIR}/cmake-build-release-docker/prime-practice" \
  "prime-practice-${VERSION}-linux-arm/mod.elf"

cp "${DIR}/README.md" \
  "prime-practice-${VERSION}-linux-arm/"

cp "${DIR}/release/gcn-static-patcher-ubuntu-24.04-arm/gcn-static-patcher-gui" \
  "prime-practice-${VERSION}-linux-arm/patcher"

tar -czvf "prime-practice-${VERSION}-linux-arm.tar.gz" \
    "prime-practice-${VERSION}-linux-arm"/*

# MacOS release
mkdir "prime-practice-${VERSION}-macos/"
cp "${DIR}/cmake-build-release-docker/prime-practice" \
  "prime-practice-${VERSION}-macos/mod.elf"

cp "${DIR}/README.md" \
  "prime-practice-${VERSION}-macos/"

cp -r "${DIR}/release/gcn-static-patcher-gui.app" \
  "prime-practice-${VERSION}-macos/patcher.app"

zip "prime-practice-${VERSION}-macos.zip" \
    -r "prime-practice-${VERSION}-macos"/*
