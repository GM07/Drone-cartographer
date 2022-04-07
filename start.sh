#!/bin/bash


xhost + local:docker &> /dev/null

docker-compose down --remove-orphans &> /dev/null

mkdir -p /tmp/socket &> /dev/null

docker rm embedded &> /dev/null 

git submodule update --init --recursive

export DOCKER_CLIENT_TIMEOUT=240
export COMPOSE_HTTP_TIMEOUT=240 

help()
{

   echo "options:"
   echo "t     Open all containers and run tests on all of them."

}

launchBackendTests(){
   docker stop test_backend  &> /dev/null 
   docker rm test_backend &> /dev/null  
   echo "Preparing backend tests" 
   docker run -d --name test_backend altaric/inf3995-backend:latest  &> /dev/null &&
   docker cp ./backend test_backend:./ &&
   docker exec -it test_backend sh -c "cd backend && pytest " 
   docker stop test_backend  &> /dev/null 
   docker rm test_backend &> /dev/null  

}

launchEmbeddedTests(){
   docker stop test_embedded  &> /dev/null
   docker rm test_embedded  &> /dev/null
   echo "Preparing embedded tests" &&
   docker run -dt --name test_embedded zedfly/inf3995:embedded  &> /dev/null &&
   docker cp ./embedded test_embedded:./  &> /dev/null &&
   docker exec -it test_embedded sh -c  "cd embedded && rm -r crazyflie-firmware && git clone --recursive https://github.com/zedfly/crazyflie-firmware.git && cd embedded-firmware/validate && rm -r build && mkdir -p build && cd build && cmake ../ && make -j`nproc`"  &> /dev/null && 
   docker exec -it test_embedded sh -c "cd embedded/embedded-firmware/validate/build && ./embedded_firmware"
  
   docker exec -it test_embedded sh -c "cd embedded/shared-firmware/validate &&  rm -r build && mkdir -p build && cd build && cmake ../ && make -j`nproc`" &> /dev/null &&
   docker exec -it test_embedded sh -c "cd embedded/shared-firmware/validate/build && ./shared_firmware" 
   
   docker exec -it test_embedded sh -c " dpkg -i embedded/argos3*.deb"  &> /dev/null &&
   docker exec -it test_embedded sh -c "cd embedded/simulation/validate &&  rm -r build && mkdir -p build && cd build && cmake ../ && make -j`nproc`" &> /dev/null  &&
   docker exec -it test_embedded sh -c "cd embedded/simulation/validate/build && ./simulation_firmware" 
   
   docker stop test_embedded  &> /dev/null  
   docker rm test_embedded  &> /dev/null  
}

launchFrontendTests(){

   docker stop test_frontend  &> /dev/null  
   docker rm test_frontend &> /dev/null  
   echo "Preparing frontend tests" &&
   docker run --name test_frontend -dt node:16.13.2-alpine3.14  &> /dev/null &&
   docker cp ./frontend test_frontend:./   &> /dev/null &&
   docker exec -it test_frontend sh -c "cd frontend && npm ci"  &> /dev/null &&
   docker exec -it test_frontend sh -c "cd frontend && npm run test" 
   docker stop test_frontend  &> /dev/null  
   docker rm test_frontend &> /dev/null  

}

launchTests(){

   launchBackendTests 
   launchEmbeddedTests 
   launchFrontendTests

}
 

while getopts ":t" option; do
   case $option in
      
      t) 
         launchTests
         exit;;
      \?) # Invalid option
         echo "Error: Invalid option"
         help
         exit;;
   esac
done



if  ! command -v nvidia-docker &> /dev/null
then
    docker-compose  up --build    
else 
    if  docker-compose --version | grep  "1.29.*" ; then
        
        
        cd ./nvidia-docker-compose &&   docker-compose  up --build  
    
    else 
        echo "Please install docker-compose 1.29"
    fi
fi
