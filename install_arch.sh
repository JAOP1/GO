#!/bin/bash
sudo pacman -S --needed sfml git cmake gtest clang gcc
rm -rf GO
wget https://download.pytorch.org/libtorch/nightly/cpu/libtorch-shared-with-deps-latest.zip
unzip libtorch-shared-with-deps-latest.zip
git clone https://github.com/JAOP1/GO
cd GO/
mkdir build
cd build
cmake ..
make