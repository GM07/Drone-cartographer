ARG VARIANT=bionic
FROM mcr.microsoft.com/vscode/devcontainers/base:${VARIANT}
LABEL Maintainer="Pierre-Yves Lajoie <pierre-yves.lajoie@polymtl.ca>"
LABEL argos-example.version="0.1"

ARG PYTHON3_VERSION=3.8

# Install common dependencies
RUN apt-get update && apt-get install -y \
	apt-utils \
	build-essential \
	dpkg \ 
	git \
	pkg-config \
	python"$PYTHON3_VERSION" \
	python"$PYTHON3_VERSION"-dev \
	python3-apt \
	python-numpy \
	python3-pip \
	libzmq3-dev \
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

# Install tools required to build crazyflie firmware and flash
RUN add-apt-repository -y ppa:team-gcc-arm-embedded/ppa && \
	apt-get update && \
	apt install -y gcc-arm-embedded \
	rake \
	openocd

# Add 3.8 to the available alternatives
RUN update-alternatives --install /usr/bin/python3 python3 /usr/bin/python"$PYTHON3_VERSION" 1

# Set python3.8 as the default python
RUN update-alternatives --set python3 /usr/bin/python"$PYTHON3_VERSION" && \
	ln -s /usr/lib/python3/dist-packages/apt_pkg.cpython-36m-x86_64-linux-gnu.so /usr/lib/python3/dist-packages/apt_pkg.so && \
	pip3 install setuptools_scm && \
	pip3 install scikit-build && \
	pip3 install --upgrade pip && \
	pip install -I pyyaml

