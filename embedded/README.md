# Installation Procedure

## Installing docker
Instructions for installing docker can be found here : https://docs.docker.com/engine/install/

After installation make to follow the post installation steps : https://docs.docker.com/engine/install/linux-postinstall/

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

# Working with GIT submodules

## Rules for submodule
THE SUBMODULES ON A BRANCH MUST POINT TO THE MAIN BRANCH OF THEIR RESPECTIVE REPOS

## Submodule introduction
Submodules are a way to have git projects inside your git project. They are basically pointers to a certain commit on another repository.
Submodules allow you to compartimentalize your projects while still tracking them with git and beeing able to work on in one place.
If you want to modify code contained in a specific submodule, you can simply navigate to that folder, do the modifications on a branch, and add them to git as if
it was a normal single repository. Branches of the repo can have submodules pointers that point on various branch of the submodule repo. Not necessarily master.

## Cloning the repository

```bash
git clone git@gitlab.com:zedfly/inf3995-temp.git --recursive
```

## Updating submodules from origin

If you pull from a branch and want to update

```bash
git submodule update --recursive
```

## Merging code split across multiple submodules
If your changes span across multiple repos you must.
1. Go to the most nested submodule you changed
2. Create an MR for the branch of that submodule
3. Checkout this submodule to the master branch
4. Navigate up to the next place you made a change
5. Add and Commit your changes that made the submodule point back to master
6. Rebase your history to clean it up from all the commits simply used to update submodules
6. Create the MR for this branch
7. Repeat procedure for other submodules/modules
8. In the MR, indicate the order by which to merge by linking the other MRs (Merge submodules Submodule before modules)
9. Before merging to master make sure all the submodules of your branch point to master!!!!!
