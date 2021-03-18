#!/bin/bash

#./PrimeAPI2/randomprime/target/debug/dol_linker_cli rel \
#  -o ./mod_test.rel \
#  -s ./PrimeAPI2/randomprime/generated/dol_symbol_table/1.00.txt \
#  cmake-build-release-llvm/prime-practice

./PrimeAPI2/randomprime/target/debug/dol_linker_cli rel \
  -o ./mod_test.rel \
  -s ./src/empty.lst \
  cmake-build-debug-llvm/prime-practice

cp rando.dol ../mp1/files/default.dol
cp rando.dol ../mp1/sys/main.dol

cp mod_test.rel ../mp1/files/patches.rel
#cp cmake-build-debug-llvm/Mod.rel ../mp1/files/patches.rel

#cp mod_test.rel ../mp1/files/Mod.rel
#cp cmake-build-release-llvm/Mod.rel ../mp1/files/Mod.rel
