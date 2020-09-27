@echo off
set VS=Visual Studio 2013
goto 0

:0
echo VS版本：
echo 1 Visual Studio 2013
echo 2 Visual Studio 2015
echo 3 Visual Studio 2017
echo 4 Visual Studio 2019

echo 请选择后回车 : 
set /p operate=
if %operate%==1 goto 1
if %operate%==2 goto 2
if %operate%==3 goto 3
if %operate%==4 goto 4

:1
set VS=Visual Studio 12 2013
goto 5
:2
set VS=Visual Studio 14 2015
goto 5
:3
set VS=Visual Studio 15 2017
goto 5
:4
set VS=Visual Studio 17 2019
goto 5

:5
echo "build glog"
cd 3rdparty/glog-0.4.0

mkdir build_win32
cd build_win32
cmake .. -G "%VS%"
cmake --build .
cmake --build . --config Release
cd ..

mkdir build_win64
cd build_win64
set Platform=Win64
set "VS64=%VS% %Platform%"
cmake .. -G "%VS64%"
cmake --build .
cmake --build . --config Release
cd ..
cd ..

echo "build gtest"
cd googletest-release-1.10.0
mkdir build_win32
cd build_win32
cmake .. -G "%VS%"
cmake --build .
cmake --build . --config Release
cd ..

mkdir build_win64
cd build_win64
set Platform=Win64
set "VS64=%VS% %Platform%"
cmake .. -G "%VS64%"
cmake --build .
cmake --build . --config Release