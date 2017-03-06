cmake_minimum_required(VERSION 2.8.12)
project(gooseeye)

add_subdirectory(pybind11)
pybind11_add_module(gooseeye image.cpp py_image.cpp)
