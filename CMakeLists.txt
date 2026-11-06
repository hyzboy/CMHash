cmake_minimum_required(VERSION 3.28)

project(CMHash LANGUAGES C CXX)

include(path_config.cmake)
CMHashSetup(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMHASH_ROOT_SOURCE_PATH})

add_subdirectory(examples)
