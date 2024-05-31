#!/bin/bash

# install the requirements
# libjpeg
echo "Installing libjpeg"
wget https://www.ijg.org/files/jpegsrc.v9f.tar.gz
tar -xvzf jpegsrc.v9f.tar.gz
rm jpegsrc.v9f.tar.gz
cd jpeg-9f

current_path=$(pwd)
./configure "CC=arm-linux-gnueabihf-gcc" --host=arm-linux-gnueabihf CC=arm-linux-gnueabihf-gcc --prefix="$current_path/build"
make
make install
cd ..

# gnu gsl
echo "Installing gnu gsl, takes a while(10-15 minutes)"
wget https://mirror.ibcp.fr/pub/gnu/gsl/gsl-latest.tar.gz
tar -xvzf gsl-latest.tar.gz
rm gsl-latest.tar.gz
mv gsl-2.8 gsl
cd gsl
./configure "CC=arm-linux-gnueabihf-gcc" --host=arm-linux-gnueabihf CC=arm-linux-gnueabihf-gcc --prefix="$current_path/build"
make 
make install
cd ..

# make app
make
