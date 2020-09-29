@echo off
set VS=Visual Studio 2013
goto 0

:0
echo VS version list
echo 1 Visual Studio 2013
echo 2 Visual Studio 2015
echo 3 Visual Studio 2017
echo 4 Visual Studio 2019

echo Input vs version : 
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

mkdir Build_Win32_vs13
cd Build_Win32_vs13
cmake .. -G "Visual Studio 12 2013"
cmake --build .
cmake --build . --config Release
cd ..

mkdir Build_x64_vs13
cd Build_x64_vs13
set Platform=Win64
set "VS64=%VS% %Platform%"
cmake .. -G "Visual Studio 12 2013 Win64"
cmake --build .
cmake --build . --config Release
cd ..

mkdir Build_Win32
cd Build_Win32
cmake .. -G "%VS%"
cmake --build .
cmake --build . --config Release
cd ..

mkdir Build_x64
cd Build_x64
set Platform=Win64
set "VS64=%VS% %Platform%"
cmake .. -G "%VS64%"
cmake --build .
cmake --build . --config Release
cd ..
cd ..




echo "build gtest"
cd googletest-release-1.10.0
mkdir Build_Win32
cd Build_Win32
cmake .. -G "%VS%" -Dgtest_force_shared_crt=ON -DBUILD_GMOCK=OFF -DINSTALL_GTEST=OFF
cmake --build .
cmake --build . --config Release
cd ..

mkdir Build_x64
cd Build_x64
set Platform=Win64
set "VS64=%VS% %Platform%"
cmake .. -G "%VS64%" -Dgtest_force_shared_crt=ON -DBUILD_GMOCK=OFF -DINSTALL_GTEST=OFF
cmake --build .
cmake --build . --config Release
cd ..
cd ..




echo "build jsoncpp"
cd jsoncpp-1.8.3
mkdir Build_Win32
cd Build_Win32
cmake .. -G "Visual Studio 12 2013"
cmake --build .
cmake --build . --config Release
cd ..

mkdir Build_x64
cd Build_x64
set Platform=Win64
set "VS64=%VS% %Platform%"
cmake .. -G "Visual Studio 12 2013 Win64"
cmake --build .
cmake --build . --config Release