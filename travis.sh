#!/usr/bin/env sh
echo "build gtest"
cd 3rdparty/googletest-release-1.10.0
mkdir build
cd build
cmake ..
make
sudo make install
cd ../../../

echo "test no boost"
mkdir build
cd build
cmake .. -DUSE_BOOST=OFF
make

cd bin
./utils_convert_sync --gtest_repeat=10
./utils_signal --gtest_repeat=10
./utils_thread --gtest_repeat=10
./utils_thread_pool --gtest_repeat=10
