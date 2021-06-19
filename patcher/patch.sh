#!/usr/bin/env bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PYTHONPATH="${SCRIPT_DIR}" python3 main.py iso -i prime.iso -o metroid-prime-practice-mod.iso -m prime-practice
