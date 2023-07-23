#ifndef H_COMMON
#define H_COMMON

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string> 
#include <cstring>

#include <optional>
#include <array>
#include <vector>
#include <set>

#include <algorithm>
#include <chrono>

#include <memory>

const uint32_t WINDOW_WIDTH = 800;
const uint32_t WINDOW_HEIGHT = 600;
const int MAX_FRAMES_IN_FLIGHT = 2;

#define IN
#define OUT
#define INOUT

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define NDEBUG
#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define PALLOCATOR		(VkAllocationCallbacks *)nullptr


#endif