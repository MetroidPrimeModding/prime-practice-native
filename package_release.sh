#!/bin/bash
set -euo pipefail

if (( $# != 4 )); then
  echo 'Usage: package_release.sh <version> <ELF> <patcher-tag> <output-directory>' >&2
  exit 1
fi

VERSION=$1
ELF=$(realpath "$2")
PATCHER_TAG=$3
OUT=$4

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PATCHER_URL="https://github.com/MetroidPrimeModding/gcn-static-patcher/releases/download/${PATCHER_TAG}"

# Each supported tag has a reviewed manifest; changing PATCHER_TAG alone must fail closed.
if [[ ! "${PATCHER_TAG}" =~ ^[A-Za-z0-9._-]+$ ]]; then
  echo "Invalid patcher tag: ${PATCHER_TAG}" >&2
  exit 1
fi
PATCHER_HASHES="${DIR}/packaging/patchers-${PATCHER_TAG}.sha256"
if [[ ! -f "${PATCHER_HASHES}" ]]; then
  echo "No pinned patcher hashes for ${PATCHER_TAG}: ${PATCHER_HASHES}" >&2
  exit 1
fi

if [ -e "${OUT}" ]; then
  echo "${OUT} already exists! Please remove it first."
  exit 1
fi
mkdir -p "${OUT}"
OUT=$(realpath "${OUT}")

PATCHERS=$(mktemp -d)
trap 'rm -rf "${PATCHERS}"' EXIT

# fetch <asset> <dir>: downloads and extracts a patcher release asset into $PATCHERS/<dir>
fetch() {
  mkdir "${PATCHERS}/$2"
  curl -fsSL -o "${PATCHERS}/$1" "${PATCHER_URL}/$1"
  local expected
  expected=$(awk -v asset="$1" '$2 == asset { print $1 }' "${PATCHER_HASHES}")
  if [[ ! "${expected}" =~ ^[0-9a-f]{64}$ ]]; then
    echo "Expected exactly one pinned SHA-256 for $1 in ${PATCHER_HASHES}" >&2
    exit 1
  fi
  # Verify the downloaded bytes before either archive tool reads them.
  printf '%s  %s\n' "${expected}" "$1" | (cd "${PATCHERS}" && sha256sum --check --strict)
  case "$1" in
    *.zip) unzip -q "${PATCHERS}/$1" -d "${PATCHERS}/$2" ;;
    *.tar.gz) tar -xzf "${PATCHERS}/$1" -C "${PATCHERS}/$2" ;;
  esac
}

# find_one <dir> <name>: the single file or directory called <name> inside $PATCHERS/<dir>
find_one() {
  local found
  found=$(find "${PATCHERS}/$1" -name "$2")
  if [ "$(echo "${found}" | grep -c .)" != 1 ]; then
    echo "Expected exactly one $2 in patcher asset $1, found: ${found:-none}" >&2
    exit 1
  fi
  echo "${found}"
}

fetch gcn-static-patcher-windows.zip windows
fetch gcn-static-patcher-linux-x86_64.tar.gz linux
fetch gcn-static-patcher-linux-arm64.tar.gz linux-arm
# Uploaded by hand (it needs signing/notarizing on a Mac), so not produced by the patcher's release workflow.
fetch gcn-static-patcher-gui-macos-universal.zip macos
# macOS-made zips can carry AppleDouble files; inside the bundle they'd break its code signature.
find "${PATCHERS}/macos" \( -name '__MACOSX' -o -name '._*' \) -prune -exec rm -rf {} +
# in case an archive dropped the executable bit
find "${PATCHERS}" -type f \( -name gcn-static-patcher-gui -o -path '*/Contents/MacOS/*' \) -exec chmod +x {} +

# package <platform> <patcher path> <patcher name in package> <zip|tar.gz>
package() {
  local name="prime-practice-${VERSION}-$1"
  mkdir "${OUT}/${name}"
  cp "${ELF}" "${OUT}/${name}/mod.elf"
  cp "${DIR}/README.md" "${OUT}/${name}/"
  cp -r "$2" "${OUT}/${name}/$3"
  (
    cd "${OUT}"
    case "$4" in
      zip) zip -qry "${name}.zip" "${name}" ;;
      tar.gz) tar -czf "${name}.tar.gz" "${name}" ;;
    esac
  )
}

cp "${ELF}" "${OUT}/prime-practice-${VERSION}.elf"

# assigned first: a failure inside $(...) passed straight as an argument wouldn't trip set -e
WINDOWS_GUI=$(find_one windows gcn-static-patcher-gui.exe)
LINUX_GUI=$(find_one linux gcn-static-patcher-gui)
LINUX_ARM_GUI=$(find_one linux-arm gcn-static-patcher-gui)
MACOS_GUI=$(find_one macos gcn-static-patcher-gui.app)

package windows "${WINDOWS_GUI}" patcher.exe zip
package linux "${LINUX_GUI}" patcher tar.gz
package linux-arm "${LINUX_ARM_GUI}" patcher tar.gz
package macos "${MACOS_GUI}" patcher.app zip
