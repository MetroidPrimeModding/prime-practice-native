#!/bin/bash -xe
# param 1: build type (Debug/Release) - default Debug
export BUILD_TYPE="${1:-Debug}"
# make build dir names lowercase (this is more cross-platform)
export BUILD_TYPE_LOWER="$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')"

export DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "${DIR}"

# these are exported so build_and_copy.sh can use them too
export IMAGE="ghcr.io/metroidprimemodding/prime-practice-native/build:latest"
export CMAKE_DIR="cmake-build-${BUILD_TYPE_LOWER}-docker" # same as my clion for convenience
export EXTERNAL_SRC_DIR="./"
export EXTERNAL_BUILD_DIR="${EXTERNAL_SRC_DIR}${CMAKE_DIR}"
export DOCKER_SRC_DIR="/tmp/prime-practice-native/"
export DOCKER_BUILD_DIR="${DOCKER_SRC_DIR}${CMAKE_DIR}"

mkdir -p "${EXTERNAL_BUILD_DIR}"

# launch a build in a docker container first (this does the same thing intellij would do)
docker run --rm -v "${EXTERNAL_SRC_DIR}":"${DOCKER_SRC_DIR}" "${IMAGE}" bash -c "cd \"${DOCKER_BUILD_DIR}\" && cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -G Ninja && cmake --build . --config ${BUILD_TYPE}"
