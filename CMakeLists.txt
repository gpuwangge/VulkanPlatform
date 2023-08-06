cmake_minimum_required(VERSION 3.20)
project(myVulkanPlatform)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug")

include_directories(
    C:/VulkanSDK 
    $ENV{VULKAN_SDK}/Include
    C:/VulkanSDK/GLFW/include
    C:/VulkanSDK/GLM)

link_directories(
    $ENV{VULKAN_SDK}/Lib
    C:/VulkanSDK/GLFW/lib-mingw-w64)

#link_libraries("/opt/MATLAB/R2012a/bin/glnxa64/libeng.so"　"/opt/MATLAB/R2012a/bin/glnxa64/libmx.so")


add_executable(simpleTriangle
    samples/simpleTriangle.cpp
    framework/application.cpp
    framework/applicationUtility.cpp
    framework/applicationController.cpp
    framework/vulkanBase.cpp
    framework/instance.cpp
    framework/debugger.cpp
    framework/physicalDevice.cpp
    framework/logicalDevice.cpp
    framework/swapchain.cpp
    framework/context.cpp
    framework/descriptor.cpp
    framework/renderProcess.cpp)

target_link_libraries(simpleTriangle vulkan-1.lib glfw3dll.lib)

add_executable(simpleUniformBuffer
    samples/simpleUniformBuffer.cpp
    framework/application.cpp
    framework/applicationUtility.cpp
    framework/applicationController.cpp
    framework/vulkanBase.cpp
    framework/instance.cpp
    framework/debugger.cpp
    framework/physicalDevice.cpp
    framework/logicalDevice.cpp
    framework/swapchain.cpp
    framework/context.cpp
    framework/descriptor.cpp
    framework/renderProcess.cpp)

target_link_libraries(simpleUniformBuffer vulkan-1.lib glfw3dll.lib)


