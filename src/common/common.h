/**
 * This is the header of every file.
 * It includes all external libraries and defines all basic types
 */
#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

// include ospray
#include "ospray/ospray.h"
#include "ospray/ospcommon/vec.h"

// include cyCodeBase here
#include <cyCore.h>
#include <cyTimer.h>
#include <cyPoint.h>
#include <cyMatrix.h>
//#include <cyGL.h>

// glm
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

// trying this obj loader https://github.com/syoyo/tinyobjloader
#include "tiny_obj_loader.h"

// include image loader
#include "lodepng.h"

// include cpp standard library
#include <algorithm>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#endif//_COMMON_H_
