cmake_minimum_required(VERSION 3.20)
project(myVulkanPlatform)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")

include_directories(
    C:/VulkanSDK
    $ENV{VULKAN_SDK}/Include
    C:/VulkanSDK/GLFW/include
    C:/VulkanSDK/GLM
    ${PROJECT_SOURCE_DIR}/framework/include
    ${PROJECT_SOURCE_DIR}/windowsFramework/include)

link_directories(
    $ENV{VULKAN_SDK}/Lib
    C:/VulkanSDK/GLFW/lib-static-ucrt)

add_subdirectory(windowsFramework)
add_subdirectory(framework)
link_libraries(vulkan-framework windows-vulkan-framework vulkan-1 glfw3dll)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

if(SINGLE)
    add_executable(simpleTriangle samples/simpleTriangle.cpp)
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

