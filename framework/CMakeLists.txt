cmake_minimum_required(VERSION 3.20)
project(myFramework)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")

aux_source_directory(${PROJECT_SOURCE_DIR} SRC)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../build)
add_library(wxj-vulkan-framework STATIC ${SRC})

