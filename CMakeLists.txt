cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Engine)
project(ftinspect)

add_subdirectory(engine)
add_subdirectory(ftinspect)

set(VS_STARTUP_PROJECT ftinspect)
