#!/bin/bash
sudo pacman -S --needed sfml git cmake gtest clang gcc
cd ..
pwd1=`pwd`

#You can replace the link by the suitable if you have GPU.
wget https://download.pytorch.org/libtorch/nightly/cpu/libtorch-shared-with-deps-latest.zip
unzip libtorch-shared-with-deps-latest.zip

cd GO-master/
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=$pwd1/libtorch ..
make
./visualizer -v true -p true