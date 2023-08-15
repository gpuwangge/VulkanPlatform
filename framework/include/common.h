#ifndef H_COMMON
#define H_COMMON

//this macro is already defined somewhere in Android
//#define ANDROID

#define GLM_FORCE_RADIANS
#ifndef ANDROID
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#else
#include "../../thirdParty/GLM/glm/glm.hpp"
#include "../../thirdParty/GLM/glm/gtc/matrix_transform.hpp"
#endif

// #define GLM_FORCE_DEPTH_ZERO_TO_ONE
// #define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/hash.hpp>
#include <unordered_map>

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string> 
#include <cstring>

#include <optional>
#include <array>
#include <vector>
#include <set>
#include <unordered_map>

#include <algorithm>
#include <chrono>

#include <memory>

//not sure usefulness...
// #include <stdexcept>
// #include <cstdlib>
// #include <cstdint>
// #include <limits>

const uint32_t WINDOW_WIDTH = 1024;
const uint32_t WINDOW_HEIGHT = 800;
const int MAX_FRAMES_IN_FLIGHT = 2;

const int MIPMAP_TEXTURE_COUNT = 7;

#define IN
#define OUT
#define INOUT

#define TEXTURE_PATH "../textures/"
#define MODEL_PATH "../models/"
#define SHADER_PATH "../shaders/"
#define ANDROID_TEXTURE_PATH "textures/"
#define ANDROID_MODEL_PATH "models/"
#define ANDROID_SHADER_PATH "shaders/"


//#define NDEBUG
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define PALLOCATOR		(VkAllocationCallbacks *)nullptr

#include <vulkan/vulkan.h>

#endif