#!/bin/bash


xhost + local:docker &> /dev/null

docker-compose down --remove-orphans &> /dev/null

mkdir /tmp/socket &> /dev/null

docker rm embedded &> /dev/null

git submodule update --init --recursive

export DOCKER_CLIENT_TIMEOUT=240
export COMPOSE_HTTP_TIMEOUT=240


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




