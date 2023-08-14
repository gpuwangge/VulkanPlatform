cmake_minimum_required(VERSION 3.20)
project(VulKanFramework)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")

aux_source_directory(${PROJECT_SOURCE_DIR}/source SRC)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../build)
add_library(vulkan-framework STATIC ${SRC})

