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

ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute

RUN apt-get update && apt-get install -y  --no-install-recommends\
    libnewlib-arm-none-eabi \
	python3-apt \
	python3-pip \
    openocd \
	&& pip3 install --upgrade pip \
    && pip3 install cfclient

COPY  ./argos3_simulator-1.0.0-x86_64-DreamTeam.deb ./
COPY  ./argos3-examples ./argos3-examples

RUN sudo dpkg -i argos3*.deb

COPY ./crazyflie-firmware  ./crazyflie-firmware
COPY ./embedded-firmware ./embedded-firmware
COPY ./shared-firmware ./shared-firmware
COPY ./simulation ./simulation

RUN cd argos3-examples && if test -d build; then rm -rf build; fi && mkdir build && cd build && cmake -GNinja -DCMAKE_BUILD_TYPE=Debug .. && ninja
