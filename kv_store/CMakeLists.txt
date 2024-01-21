cmake_minimum_required(VERSION 3.2)

project(processor)

add_library(processor SHARED Processor.cpp SkipList.cpp)
