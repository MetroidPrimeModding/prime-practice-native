#!/bin/bash
# Prints the README.md changelog entry for <version>; fails if there isn't one.
# Headings look like "### 2.11.0", "### 2.10.0/2.10.1" or "### 2.3.0 (beta)".
# usage: release_notes.sh <version>
set -euo pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

NOTES=$(awk -v version="$1" '
  /^## / { in_changelog = ($0 == "## Changelog"); if (found) exit; next }
  /^### / {
    if (found) exit
    n = split(substr($0, 5), parts, /[\/ ]/)
    for (i = 1; i <= n; i++) if (in_changelog && parts[i] == version) found = 1
    next
  }
  found { print }
' "${DIR}/README.md")

if [ -z "$(echo "${NOTES}" | tr -d '[:space:]')" ]; then
  echo "No changelog entry for $1 in README.md" >&2
  exit 1
fi
echo "${NOTES}"
