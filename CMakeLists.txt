cmake_minimum_required(VERSION 3.20)
SET(CMAKE_C_COMPILER "C:/mingw64/bin/gcc.exe")
SET(CMAKE_CXX_COMPILER "C:/mingw64/bin/g++.exe")
project(VulkanPlatform)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")

include_directories(
    $ENV{VULKAN_SDK}/Include
    $ENV{INCLUDE}        #Put this in environment user variable INCLUDE: C:\VulkanSDK\GLFW\include;C:\VulkanSDK\GLM
    ${PROJECT_SOURCE_DIR}/framework/include
    ${PROJECT_SOURCE_DIR}/windowsFramework/include)

link_directories(
    $ENV{VULKAN_SDK}/Lib
    $ENV{LIB})          #Put this in environment user variable LIB: C:\VulkanSDK\GLFW\lib-static-ucrt  

#to verify environment varaibles (reboot machine after change if needed):
#echo %VULKAN_SDK%
#echo %INCLUDE%
#echo %LIB%


add_subdirectory(windowsFramework)
add_subdirectory(framework)
link_libraries(vulkan-framework windows-vulkan-framework vulkan-1 glfw3dll)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

if(SINGLE)
    set(SAMPLE_NAME simpleCompute)
    add_executable(${SAMPLE_NAME} samples/${SAMPLE_NAME}.cpp)
else()
    aux_source_directory(${PROJECT_SOURCE_DIR}/samples SRC)
    foreach(sampleFile IN LISTS SRC)
        get_filename_component(sampleName ${sampleFile} NAME_WE)
        add_executable(${sampleName} ${sampleFile})
    endforeach()
endif()










#test link to a specific exe
# add_executable(simpleTriangle samples/simpleTriangle.cpp)
# target_link_libraries(simpleTriangle wxj-vulkan-framework vulkan-1 glfw3dll)

#test work code   
# aux_source_directory(${PROJECT_SOURCE_DIR}/framework SRC) 
# add_executable(testfw samples/simpleTriangle.cpp ${SRC})
# target_link_libraries(testfw vulkan-1 glfw3dll)

#test work code
# aux_source_directory(${PROJECT_SOURCE_DIR}/framework SRC)
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
# add_library(fw STATIC ${SRC})
# add_executable(testfw samples/simpleTriangle.cpp)
# target_link_libraries(testfw fw vulkan-1 glfw3dll)

