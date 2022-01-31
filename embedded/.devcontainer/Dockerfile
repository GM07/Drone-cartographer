FROM mcr.microsoft.com/vscode/devcontainers/cpp:focal

# Install all dependencies
RUN apt-get update \
    && apt-get install -y --no-install-recommends\
    freeglut3-dev \
    qt5-default \
    libxi-dev \
    libxmu-dev \
    libfreeimage-dev \
    libfreeimageplus-dev \
    liblua5.3-dev \
    lua5.3 \
    libga-dev \
    gcc-arm-none-eabi \
    ninja-build \
    libboost-system-dev \
    libboost-thread-dev \
    libboost-filesystem-dev \
    pkg-config \
    clang-tidy \
    clang-format \
    rake \
    libglvnd0 \
    libgl1 \
    libglx0 \
    libegl1 \
    libxext6 \
    libx11-6

# Set env variable for 
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute

RUN apt-get update && apt-get install -y \
    libnewlib-arm-none-eabi \
	python3-apt \
	python3-pip \
    openocd \
	&& pip3 install --upgrade pip \
    && pip3 install cfclient