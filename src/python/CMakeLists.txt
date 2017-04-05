cmake_minimum_required(VERSION 2.8.12)
project(gooseeye)

set (CMAKE_CXX_STANDARD 14)

add_subdirectory(pybind11)
pybind11_add_module(gooseeye image.cpp ../core/image.cpp)
