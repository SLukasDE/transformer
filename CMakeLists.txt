# CMakeLists.txt
cmake_minimum_required(VERSION 3.23)

project(
    tbuild
    VERSION 1.0.0
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src/main)
