echo "Installing BOOST libraries"
sudo apt-get install libboost-dev

mkdir build-Debug
cd build-Debug

cmake ../src -DCMAKE_BUILD_TYPE=Debug
make -j
