export CC=/usr/local/bin/gcc-4.8
export CXX=/usr/local/bin/g++-4.8

mkdir build-gcc-4.8
cd build-gcc-4.8
cmake ../src
make
