# Demonstration of the requirements

## SIMULATION
#### R.C.3 : https://youtu.be/ZTsq-fo1JRo
#### R.C.5 : https://youtu.be/MNa5B4uPBEc
#### 
#### R.F.2 : https://youtu.be/exyVJGXXqWs 
#### R.F.3 : https://youtu.be/r0D-aA17_5Q
#### R.F.4-R.F.5 :  https://youtu.be/Kp2DKjgZOGI
#### R.F.6 : https://youtu.be/hazaTemEUx8
#### R.F.7 : https://youtu.be/lQY7RHeyr5o
#### R.F.8-R.F.9 : https://youtu.be/OxlJ8gVUlok
#### R.F.11 : https://youtu.be/UEtxdsRyZWE
#### R.F.12 : https://youtu.be/nQvFE3vZH98
#### R.F.16 : https://youtu.be/NkFpbyQ3oqQ

## PHYSIQUE
#### R.F.1 : https://youtu.be/jW5N3KHpleM
#### R.F.2 : https://youtu.be/910Vv9pmuNg
#### R.F.3 : https://youtu.be/zdT6yJBZk_A
#### R.F.4-R.F.5 : https://youtu.be/xhmPRty4eb4
#### R.F.6-8-9-11-12 : https://youtu.be/B7L6vY_rDgE
#### R.F.7 : https://youtube.com/shorts/DulzTUlVoj8?feature=share
#### R.F.13 : https://youtu.be/Dh6XQaVc9tI
#### R.F.14-R.F.16 : https://youtu.be/wPQ9SxHwMvI
#### R.F.19 : https://youtu.be/PuRP_JvzIHs

## AUTRES
#### R.C.1 : https://youtu.be/0A95FmTyeaU
#### R.F.10 : https://youtu.be/eavHEMG_ItM  


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
To get a detailed explanation of these steps please visit the official [Nvidia documentation](https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html)

### Docker-compose:
You must also install docker-compose
```
sudo curl -L "https://github.com/docker/compose/releases/download/1.29.2/docker-compose-$(uname -s)-$(uname -m)" -o /usr/local/bin/docker-compose
```
The minimum required version of docker-composed is 1.25.* unless you are using nvidia-docker then you must at least install version 1.29.* 

Additional information about the installation is provided:[Docker-Compose documentation](https://docs.docker.com/compose/install/)

## Automatic Launch
In order to launch the application you can simply execute the shell script present in the repository
```
./start.sh
```
The script will launch and configure all of the Docker containers

If the file is not recognized as an executable please run this next command from the root of the project
```
chmod u+x start.sh
```

## Running tests

You can test each component of the project by running the start script with the test option (-t).
```
./start.sh -t
```
The test results will then be available in the terminal.
## Manual Launch

This next part explains how to manually launch and configure the containers as Docker devcontainers

### Graphic Card Configuration (OPTIONAL)
If you are using an NVIDIA GPU you must install additionnal software for the simulation to display properly in the container

Follow the instructions provided here : https://docs.nvidia.com/datacenter/cloud-native/container-toolkit/install-guide.html (NOTE it is not necessary to reinstall Docker)

You also need to modify the `devcontainer.json` file located in `INF3995-106/embedded/.devcontainer/` by replacing the `runArgs` section with the following

```json
"runArgs": [
	  	//"--privileged",
	    //"--net=host",
		"--device=/dev/ttyUSB0",
		"--gpus=all", //Uncomment to use NVDIA GPU
		"-e",
		"DISPLAY=${env:DISPLAY}",
	],
```


### Manually Lauching the project
To launch all the components of the project, you need the folders `backend`, `frontend` and `embedded` opened in vscode. Then using the Remote-Explorer extension from vscode, re-open all three folders in their respective container.

### Launching the Client
In the devcontainer for the `frontend` directory. Once the container is built, open a terminal window inside vscode and run the command

```bash
npm run serve
```

Once the compilation is done, the client can now be accessed in a web-browser at `http://localhost:8080`

### Lauching the Server
In the devcontainer for the `frontend` directory. Once the container is built, open a terminal widow inside vscode and run the command

```bash
python3 server.py
```
This will start the server and allow the client to connect to it.

### Launching the simulation
In the devcontainer for the `embedded` directory.

Once the container is built, hit `CTRL-SHIFT-P`, select `Tasks: Run Task` and run the `Install Argos3 Package` Task

Afterwards, the same way as above, run the `Compile Simulation Firmware` Task

Then make sure the server and client are in their inital state (i.e. Have just been launched)

On the client, check the `Simulation Toggle` and press the `Démarrer mission` button

In the `embedded` devcontainer open a terminal inside vscode and run the commands 
```bash
cd simulation
argos3 -c experiments/crazyflie_sensing.argos
```

A simulation window should pop-up

You can now start the simulation using the `Play` button and send commands to the drone with the `Lancer Mission` and `Terminer mission` buttons on the client.

### Flashing the Drone
Make sure the CrazyRadio is connected to your computer.

In the devcontainer for the `embedded` directory, hit `CTRL-SHIFT-P` and select `Remote-Containers: Rebuild Container`

Once the container is re-built, hit `CTRL-SHIFT-P`, select `Tasks: Run Task` and run the `Flash embedded firmware` Task

The drones should now be flashed

### Sending commands to the physical drones
Make sure the CrazyRadio is connected to your computer.

In the devcontainer for the `server` directory, hit `CTRL-SHIFT-P` and select `Remote-Containers: Rebuild Container`

Launch the client (See Launching the Client)
Launch the server (See Launching the Server)

Make sure the drones have been flashed (See Flashing the Drone) and are powered on

On the client, do not check the `Simulation Toggle` button and press the `Démarrer mission` button

You should be able to select drone radio addresses in the top bar of the UI on the client. Selecting a Drone will allow you to send the identify command which will make its LED blink.

