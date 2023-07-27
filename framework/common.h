#ifndef H_COMMON
#define H_COMMON

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

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

const uint32_t WINDOW_WIDTH = 800;
const uint32_t WINDOW_HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

const int MIPMAP_TEXTURE_COUNT = 7;

#define IN
#define OUT
#define INOUT


#define NDEBUG
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define PALLOCATOR		(VkAllocationCallbacks *)nullptr

#include "debugger.h"


#endif