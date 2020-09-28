cmake_minimum_required (VERSION 3.5)

project (commonex)


option (USE_BOOST "Not support C11, need boost" ON)

#boost
if(USE_BOOST)
    if (NOT BOOST_DIR)
        set(BOOST_DIR D:/Workspace/externals_bak/boost_1_73_0)
    endif ()
    include_directories(${BOOST_DIR})
    link_directories(${BOOST_DIR}/stage/lib)
endif()

message(STATUS "PROJECT_SOURCE_DIR " ${PROJECT_SOURCE_DIR})
message(STATUS "PROJECT_BINARY_DIR " ${PROJECT_BINARY_DIR})

configure_file (
    config.h.in 
    ${PROJECT_SOURCE_DIR}/include/config.h
)

set(GTEST_DIR 3rdparty/googletest-release-1.10.0)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


include_directories(include)
include_directories(${GTEST_DIR}/googletest/include)

if(WIN32)
    if(CMAKE_CL_64)
        link_directories(${GTEST_DIR}/build_x64/lib)
    else()
        link_directories(${GTEST_DIR}/build_Win32/lib)
    endif()
else()
    link_directories(/usr/local/lib)
endif()

add_definitions(-std=c++11)

link_libraries(gtest gtest_main)
if(UNIX)
    link_libraries(pthread)
    if(USE_BOOST)
        link_libraries(boost_system boost_thread boost_timer boost_chrono)
    endif()
endif()

add_executable(utils_convert_sync include/test/utils_convert_sync)

add_executable(utils_signal include/test/utils_signal)

add_executable(utils_thread include/test/utils_thread)
