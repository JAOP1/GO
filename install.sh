#!/bin/bash

cd ..
pwd1=`pwd`

#Check the OS.
is_ubuntu=$(cat /etc/os-release | grep -c Ubuntu)
is_manjaro=$(cat /etc/os-release | grep -c Manjaro)
is_valid_OS=true

if [ $is_manjaro -gt 0 ]
then
    echo The current operative system is Manjaro.
    sudo pacman -S --needed sfml git cmake gtest clang gcc

elif [ $is_ubuntu -gt 0 ]
then 
    echo The current operative system is Ubuntu.
    #It works in Ubuntu1710. 
    #It was copied of the mraggi repository.
    # Remove all traces of previous SFML installation
    sudo apt-get remove libsfml-dev
    sudo rm -rf /usr/local/share/SFML
    sudo rm -rf /usr/local/include/SFML
    sudo rm -rf /usr/local/lib/*sfml*
    sudo rm -rf /usr/share/SFML
    sudo rm -rf /usr/include/SFML
    sudo rm -rf /usr/lib/*sfml*

    # Basic things
    sudo apt-get -y install build-essential mc cmake git software-properties-common python-software-properties libboost-dev libgtest-dev  


    sudo apt-get -y install libsfml-dev

    cd /usr/src/gtest/
    sudo cmake .
    sudo make
    sudo cp *.a /usr/lib

    cd $pwd1
else
    echo You should install the requirements manual.
    is_valid_OS=false
fi

if [ is_valid_OS ]
then 
    #You can replace the link by the suitable if you have GPU.
    wget https://download.pytorch.org/libtorch/nightly/cpu/libtorch-shared-with-deps-latest.zip
    unzip libtorch-shared-with-deps-latest.zip

    cd GO-master/
    mkdir build
    cd build
    cmake -DCMAKE_PREFIX_PATH=$pwd1/libtorch ..
    make
    ./visualizer -v true -p true
fi