#!/bin/bash

cd ..
pwd1=`pwd`

#Check the OS.
is_ubuntu=$(cat /etc/os-release | grep -c Ubuntu)
is_manjaro=$(cat /etc/os-release | grep -c Manjaro)


if [ $is_manjaro -gt 0 ]
then
    echo The current operative system is Manjaro.
    sudo pacman -S --needed sfml git cmake gtest clang gcc boost 

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

