FROM ubuntu:24.04

# Prevent interactive prompts during installation
ENV DEBIAN_FRONTEND=noninteractive

# LLVM version. C++26 support improves with every release, so keep this current.
ARG LLVM_VERSION=23

# Install essential dependencies, CMake, Ninja, gdb, and LLVM (clang, clangd,
# clang-tidy, clang-format, and libc++ — LLVM's standard library, which has the
# most complete C++26 library support for clang).
RUN apt-get update && apt-get install -y \
    wget \
    gnupg \
    lsb-release \
    software-properties-common \
    git \
    build-essential \
    cmake \
    ninja-build \
    gdb \
    && wget -O /tmp/llvm.sh https://apt.llvm.org/llvm.sh \
    && chmod +x /tmp/llvm.sh \
    && /tmp/llvm.sh ${LLVM_VERSION} all \
    && rm /tmp/llvm.sh \
    && apt-get clean && rm -rf /var/lib/apt/lists/*

# Set Clang as the default compiler
ENV CC=clang-${LLVM_VERSION}
ENV CXX=clang++-${LLVM_VERSION}

# Unversioned names for the LLVM tools (used by VS Code, presets, tasks, and CI)
RUN for tool in clang clang++ clangd clang-format clang-tidy run-clang-tidy; do \
      ln -sf /usr/bin/${tool}-${LLVM_VERSION} /usr/bin/${tool}; \
    done
