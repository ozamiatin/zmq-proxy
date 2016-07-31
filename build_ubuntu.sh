echo "Installing cmake 3.2.2"

sudo apt-get install -qq --force-yes build-essential
wget http://www.cmake.org/files/v3.2/cmake-3.2.2.tar.gz
tar xf cmake-3.2.2.tar.gz
cd cmake-3.2.2
./configure
make -j && sudo make install && sudo ldconfig

# sudo apt-get install -qq --force-yes cmake

#echo "Installing BOOST libraries"
#sudo apt-get install -qq --force-yes libboost-dev libboost-program-options-dev

#echo "Installing ZeroMQ"
#sudo apt-get install -qq --force-yes libzmq5-dev

mkdir build-Debug
cd build-Debug

cmake ../src -DCMAKE_BUILD_TYPE=Debug
make -j
