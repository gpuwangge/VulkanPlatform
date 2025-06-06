cmake_minimum_required(VERSION 3.20)
SET(CMAKE_C_COMPILER "C:/mingw64/bin/gcc.exe")
SET(CMAKE_CXX_COMPILER "C:/mingw64/bin/g++.exe")
project(VulkanPlatform)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")

add_definitions(-DSDL)

include_directories(
    $ENV{VULKAN_SDK}/Include
    $ENV{INCLUDE}
    ${PROJECT_SOURCE_DIR}/vulkanFramework/include
    ${PROJECT_SOURCE_DIR}/thirdparty)

link_directories(
    $ENV{VULKAN_SDK}/Lib #Put this in environment user variable LIB: C:\VulkanSDK\GLFW\lib-static-ucrt  
    $ENV{LIB}
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty #put SDL3.dll and glfw3.dll in thirdparty/
    ${PROJECT_SOURCE_DIR}/thirdparty/yaml-cpp/lib) 

add_subdirectory(sdlframework)
add_subdirectory(vulkanFramework)
link_libraries(vulkan-framework sdl-vulkan-framework vulkan-1 SDL3 yaml-cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

if(SINGLE)
    set(SAMPLE_NAME gemmCompute)
    add_executable(${SAMPLE_NAME} samples/${SAMPLE_NAME}.cpp)
else()
    file(WRITE ${EXECUTABLE_OUTPUT_PATH}/autotest.bat "
")
    
    aux_source_directory(${PROJECT_SOURCE_DIR}/samples SRC)
    foreach(sampleFile IN LISTS SRC)
        get_filename_component(sampleName ${sampleFile} NAME_WE)
        add_executable(${sampleName} ${sampleFile})
        file(APPEND ${EXECUTABLE_OUTPUT_PATH}/autotest.bat "start /wait ${sampleName}
")
    endforeach()
endif()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/sdl3.dll DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/yaml-cpp/lib/libyaml-cpp.dll DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
