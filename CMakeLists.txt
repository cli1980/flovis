cmake_minimum_required(VERSION 3.0)
project(FlowVisualizer)

set(CMAKE_CXX_FLAGS "-std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")


find_package(OpenCV 4.0 REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(flovis main.cpp flow_visualizer.cpp)
target_link_libraries(flovis ${OpenCV_LIBS})
