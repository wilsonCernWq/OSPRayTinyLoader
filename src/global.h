#pragma once
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "common.h"
#include "helper.h"

#include "world.h"
#include "meshwrapper.h"

// Global Variables
namespace otv 
{
  // objects
  extern World world;  
  extern Mesh mesh;
  // flags
  extern bool NOWIN_FLAG;
};

#endif//_GLOBAL_H_
