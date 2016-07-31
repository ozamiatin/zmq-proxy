#echo "Installing BOOST libraries"
#sudo apt-get install libboost-dev

echo "Installing cmake"
sudo apt-get install cmake

mkdir build-Debug
cd build-Debug

cmake ../src -DCMAKE_BUILD_TYPE=Debug
make -j
