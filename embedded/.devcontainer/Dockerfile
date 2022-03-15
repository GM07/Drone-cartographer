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

RUN apt-get update && apt-get install -y libncurses5

RUN apt-get install -y gcc-arm-none-eabi gdb-multiarch
RUN sudo ln -s /usr/bin/gdb-multiarch /usr/local/bin/arm-none-eabi-gdb
RUN sudo apt-get install -y usbutils

ARG GNU_ARM_VERSION=10-2020-q4-major
ARG GNU_ARM_DIRECTORY=/opt/toolchains
RUN mkdir -p ${GNU_ARM_DIRECTORY} && \
    wget -qO- "https://developer.arm.com/-/media/Files/downloads/gnu-rm/10-2020q4/gcc-arm-none-eabi-${GNU_ARM_VERSION}-x86_64-linux.tar.bz2" | tar -xj -C "${GNU_ARM_DIRECTORY}"

ENV PATH="${GNU_ARM_DIRECTORY}/gcc-arm-none-eabi-${GNU_ARM_VERSION}/bin:$PATH"

RUN apt-get install -y python3-tk