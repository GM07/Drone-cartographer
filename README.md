# Project Configuration for Ubuntu

## Cloning project

This command requires a valid ssh key that is registered in GitLab

```
git clone git@gitlab.com:polytechnique-montr-al/inf3995/20221/equipe-106/INF3995-106.git --recursive
```

## Installing Dependencies
### Docker:
You have to install docker
```
    sudo apt-get install docker-ce docker-ce-cli containerd.io
```
You will also need nvidia-docker if you have a nvidia gpu that is being used on ubuntu 
```
distribution=$(. /etc/os-release;echo $ID$VERSION_ID) \
   && curl -s -L https://nvidia.github.io/nvidia-docker/gpgkey | sudo apt-key add - \
   && curl -s -L https://nvidia.github.io/nvidia-docker/$distribution/nvidia-docker.list | sudo tee /etc/apt/sources.list.d/nvidia-docker.list
   sudo apt-get update
   sudo apt-get install -y nvidia-docker2
   sudo systemctl restart docker
```
You can then test it with this command
```
sudo docker run --rm --gpus all nvidia/cuda:11.0-base nvidia-smi
```
To get a detailed explanation of these steps please visit the official nvidia documentation : https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html

### Docker-compose:
You must also install docker-compose
```
sudo curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
```
The minimum required version of docker-composed is 1.25.* unless you are using nvidia-docker then you must at least install version 1.29.* 

Here is the documentation for the installation : https://docs.docker.com/compose/install/

## Launching the project 

The project can be launched by executing the bash script inside the root of the project
```
./start.sh
```
If you are unable to start the script please run this command from the root of the project
```
chmod +wx start.sh
```
Once the containers are done being created the web application will be accessible from http://localhost:8080/ 