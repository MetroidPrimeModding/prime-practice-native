#!/bin/bash -xe

# call build with all params; this will set some env vars we use later
source ./build.sh "$@"

${CONTAINER_COMMAND} run --rm -v "${EXTERNAL_SRC_DIR}":"${DOCKER_SRC_DIR}":z "${IMAGE}" bash -c "\
  gcn-static-patcher-cli \
    -m \"${DOCKER_BUILD_DIR}/prime-practice\" \
    -i \"${DOCKER_SRC_DIR}/prime.iso\" \
    -o \"${DOCKER_SRC_DIR}/prime-practice-mod.iso\" \
    --overwrite"
