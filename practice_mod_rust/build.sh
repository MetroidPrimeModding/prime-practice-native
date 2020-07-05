#!/bin/bash -e

cargo +nightly rustc --release --target powerpc-unknown-linux-gnu -- -C relocation-model=static
cbindgen --config cbindgen.toml --crate practice_mod_rust --output ../src/include/practice_mod_rust.h