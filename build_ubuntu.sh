#echo "Installing BOOST libraries"
#sudo apt-get install -qq --force-yes libboost-dev
sudo apt-get install -qq --force-yes libboost-program-options-dev

# echo "Installing cmake"
# sudo apt-get install -qq --force-yes cmake

mkdir build-Debug
cd build-Debug

cmake ../src -DCMAKE_BUILD_TYPE=Debug
make -j
