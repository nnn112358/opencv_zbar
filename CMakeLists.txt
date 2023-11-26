
#SET(CMAKE_BUILD_TYPE "Debug")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
PROJECT(example)
cmake_minimum_required(VERSION 3.5)


if(TARGET_COMPILER STREQUAL "arm")
    message(STATUS "TARGET_COMPILE STREQUAL arm")
    SET(CMAKE_CXX_COMPILER arm-none-linux-gnueabihf-g++)
    SET(CMAKE_C_COMPILER arm-none-linux-gnueabihf-gcc)
    SET(NCNN_INSTALL_DIR /opt/external/ncnn/install/arm/)
    SET(ZBAR_INSTALL_DIR /opt/external/zbar-0.10/build/arm)
    SET(OpenCV_DIR /opt/external/opencv/build/arm)
else()
    message(STATUS "TARGET_COMPILE STREQUAL X64")
    SET(NCNN_INSTALL_DIR /opt/external/ncnn/install/x64/)
    SET(ZBAR_INSTALL_DIR /opt/external/zbar-0.10/build/x64/)
    SET(OpenCV_DIR /opt/external/opencv/build/x64)
endif()

if(TARGET_COMPILER STREQUAL "arm")
    message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY arm")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/arm)
else()
    message(STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY X64")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/x64)
endif()

set(Target z_bar_opencv)

find_package(OpenCV REQUIRED)
add_executable(${Target} main.cpp)
link_directories(${OpenCV_LIBRARY_DIRS})

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${OpenCV_INCLUDE_DIRS} )
include_directories(${NCNN_INSTALL_DIR}/include/ncnn)
include_directories(${ZBAR_INSTALL_DIR}/include/)
target_link_libraries(${Target} ${OpenCV_LIBRARIES})
target_link_libraries (${Target} ${NCNN_INSTALL_DIR}/lib/libncnn.a -fopenmp)
target_link_libraries (${Target} ${ZBAR_INSTALL_DIR}/lib/libzbar.so)



