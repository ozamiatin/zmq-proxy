# echo "Installing cmake"
# sudo apt-get install -qq --force-yes cmake

#echo "Installing BOOST libraries"
#sudo apt-get install -qq --force-yes libboost-dev libboost-program-options-dev

#echo "Installing ZeroMQ"
#sudo apt-get install -qq --force-yes libzmq5-dev

mkdir build-Debug
cd build-Debug

cmake ../src -DCMAKE_BUILD_TYPE=Debug
make -j
