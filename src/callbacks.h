/**
 * This file defines all helper functions.
 * Those functions shouldn't depend on any thing in this project
 */
#pragma once
#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include "common.h"
#include "helper.h"

#include "world.h"
#include "meshwrapper.h"

// Global Variables
namespace otv 
{
  // world
  extern World world;
  
  // mesh object
  extern Mesh mesh;
};

namespace otv 
{
  void GetNormalKeys(unsigned char key, GLint x, GLint y);
  void GetSpecialKeys(int key, GLint x, GLint y);
  void GetMouseButton(GLint button, GLint state, GLint x, GLint y);
  void GetMousePosition(GLint x, GLint y);
  inline void Idle() { glutPostRedisplay(); }
};

#endif//_CALLBACKS_H_
