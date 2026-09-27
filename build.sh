#!/bin/bash -xe
# param 1: build type (Debug/Release) - default Debug
BUILD_TYPE="${1:-Debug}"
# make build dir names lowercase (this is more cross-platform)
BUILD_TYPE_LOWER="$(echo "$BUILD_TYPE" | tr '[:upper:]' '[:lower:]')"

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "${DIR}"

# these are exported so build_and_copy.sh can use them too
# keep in sync with the container image in .github/workflows/release.yml
IMAGE="ghcr.io/metroidprimemodding/gcn-static-patcher/build:20260927"
CMAKE_DIR="cmake-build-${BUILD_TYPE_LOWER}-docker" # same as my clion for convenience
EXTERNAL_SRC_DIR="./"
EXTERNAL_BUILD_DIR="${EXTERNAL_SRC_DIR}${CMAKE_DIR}"
DOCKER_SRC_DIR="/tmp/prime-practice-native/"
DOCKER_BUILD_DIR="${DOCKER_SRC_DIR}${CMAKE_DIR}"

mkdir -p "${EXTERNAL_BUILD_DIR}"

CONTAINER_COMMAND=docker
if command -v podman; then
  CONTAINER_COMMAND=podman
fi

# launch a build in a docker container first (this does the same thing intellij would do)
${CONTAINER_COMMAND} run --rm -v "${EXTERNAL_SRC_DIR}":"${DOCKER_SRC_DIR}":z "${IMAGE}" bash -xec "cd \"${DOCKER_BUILD_DIR}\" && cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} -G Ninja && cmake --build . --config ${BUILD_TYPE}"

# carveout/stomp fill report from the pack step (it only prints during the build when packing reruns)
set +x
cat "${EXTERNAL_BUILD_DIR}/packed.txt"
