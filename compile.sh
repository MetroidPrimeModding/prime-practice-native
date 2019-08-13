#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

python3 ./PrimeAPI/script/BuildModule.py . default.dol -v
ret=$?
if [ $ret -eq 0 ]; then
  cp -v default_mod.dol ../mp1/files/default.dol
  cp -v default_mod.dol ../mp1/sys/main.dol
  cp -v build/Mod.rel ../mp1/files/Mod.rel
else
  echo Failed to compile module
fi
