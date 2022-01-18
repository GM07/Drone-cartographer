# Installation Procedure

## Installing docker
Instructions for installing docker can be found here : https://docs.docker.com/engine/install/

After installation make to follow the post installation steps : https://docs.docker.com/engine/install/linux-postinstall/

## Cloning the repository

```bash
git clone git@gitlab.com:zedfly/inf3995-temp.git --recursive
```

## Rendering GUI with NVDIA GPU (ONLY IF APPLICABLE) 
To use NVDIA GPU with docker you need to install NVIDIA Container Toolkit  
Follow the instructions presented here to do so : https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html

Replace your devcontainer.json with the following
```json
{
	"name": "INF3995 - Dream Team",
	"dockerFile": "./Dockerfile",
	"settings": {
		"terminal.integrated.defaultProfile.linux": "zsh",
	},
	"extensions": [
		"ms-vscode.cpptools",
		"xaver.clang-format",
		"visualstudioexptteam.vscodeintellicode",
		"zixuanwang.linkerscript",
		"cschlosser.doxdocgen",
		"pkief.material-icon-theme",
		"editorconfig.editorconfig",
		"mhutchie.git-graph"
	],
	"workspaceFolder": "/workspaces/INF3995",
	"runArgs": [
		"--gpus=all",
		"-e", "DISPLAY=${env:DISPLAY}",
	],
	"mounts": [
		"source=/tmp/.X11-unix,target=/tmp/.X11-unix,type=bind",
	],
	"remoteEnv": {
		"QT_X11_NO_MITSHM": "1"
	},
	"initializeCommand": "xhost +local:docker"
}
```

Add the following to your Dockerfile
```docker
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
```
## Running the container
1. Open the folder in VSCode  
2. Make sure you have the remote container extension installed  
3. Open the command palette (CTRL-SHIFT-P) and run 'Remote-Containers: Open Folder in Containers'

## Setting up Argos simulation
1. Open the commant palette (CTRL-SHIFT-P)  
2. Select 'Tasks: Run Task'  
3. Select 'Setup Simulation'

## Running argos example
1. Navigate to the 'argos-examples' directory
2. ```bash
    argos3 -c experiments/crazyflie_sensing.argos
     ```
