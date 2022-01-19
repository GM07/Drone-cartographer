ARG VARIANT=bionic
FROM mcr.microsoft.com/vscode/devcontainers/base:${VARIANT}
LABEL Maintainer="Pierre-Yves Lajoie <pierre-yves.lajoie@polymtl.ca>"
LABEL argos-example.version="0.1"

# Install common dependencies
RUN apt-get update && apt-get install -y \
    apt-utils \
    build-essential \
    dpkg \ 
    git \
    pkg-config \
    python \
    python-dev \
    python-numpy \
    sudo \
    software-properties-common \
    && rm -rf /var/lib/apt/lists/*

# Allow usage of X11 and NVDIA GPU 
# Dependencies for glvnd and X11.
RUN apt-get update \
    && apt-get install -y -qq --no-install-recommends \
    libglvnd0 \
    libgl1 \
    libglx0 \
    libegl1 \
    libxext6 \
    libx11-6 \
    && rm -rf /var/lib/apt/lists/*
# Env vars for the nvidia-container-runtime.
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute


# Install GCC 10 since the default version is GCC 7
RUN add-apt-repository ppa:ubuntu-toolchain-r/test && \
    apt-get update && \
    apt-get -y install gcc-10 \
    g++-10 

# Install CLANG-13 to support clang-format
RUN bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

# Update symlinks to use both gcc-10 and clang-13
RUN ln -f -s /usr/bin/gcc-10 /usr/bin/gcc && \
    ln -f -s /usr/bin/g++-10 /usr/bin/g++ && \
    ln -f -s /usr/bin/clang-13 /usr/bin/clang && \
    ln -f -s /usr/bin/clang++-13 /usr/bin/clang++

# Install clang-tidy
RUN apt-get update && \
    apt-get -y install clang-tidy \
    clang-format

# Install ARGoS dependencies
RUN apt-get update && apt-get install -y \
    wget \
    freeglut3-dev \
    qt5-default \
    libxi-dev \
    libxmu-dev \
    libfreeimage-dev \
    libfreeimageplus-dev \
    liblua5.2-dev \
    lua5.2 \
    liblua5.3-dev \
    lua5.3 \
    libboost-filesystem-dev \
    cmake \
    ninja-build \
    && rm -rf /var/lib/apt/lists/*

# Install tools required to build crazyflie firmware
RUN add-apt-repository -y ppa:team-gcc-arm-embedded/ppa && \
    apt-get update &&\
    apt install -y gcc-arm-embedded \
    rake