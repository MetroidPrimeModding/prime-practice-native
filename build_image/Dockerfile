FROM --platform=$BUILDPLATFORM ghcr.io/metroidprimemodding/gcn-static-patcher/gcn-static-patcher:latest as gcn-static-patcher
LABEL authors="pwootage"

FROM --platform=$BUILDPLATFORM devkitpro/devkitppc:20251117 as build-image
LABEL authors="pwootage"

# Need to install llvm, as practice mod also requires it

RUN apt-get update && apt-get upgrade -y && \
    apt-get install -y --no-install-recommends llvm-19 llvm-19-tools clang-19 lld-19 ninja-build && \
    rm -rf /var/lib/apt/lists/* /tmp/* /var/tmp/* && \
    apt-get -y autoremove --purge && \
    apt-get -y clean

# Copy gcn-static-patcher from its build stage
COPY --from=gcn-static-patcher /opt/gcn-static-patcher /opt/gcn-static-patcher

ENV LLVM_DIR=/usr/lib/llvm-19
ENV LLD_DIR=/usr/lib/llvm-19
