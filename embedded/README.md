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
Uncomment the '--gpus=all' line from the devcontainer.json  

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
