#pragma once
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "common/helper.h"
#include "loader/meshwrapper.h"
#include "ospray/world.h"
#include "scenegraph.h"

// Global Variables
namespace otv 
{
  /* global objects */

  extern World world;
  extern std::vector<Mesh*> meshes;
  
  /* global parameters */

  extern bool  NOWIN;
  extern int   WINX, WINY;
  extern vec2i WINSIZE;
  
  /* global functions */

  //! @name Initialize the whole syste,
  //! this function should be defined by openGL backend
  void Init(int argc, const char **argv);

  //! @name Create ospray and openGL contexts
  //! defined in global.cpp  
  void Create(int argc, const char **argv);

  //! @name Cleanup everything 
  //! defined in global.cpp  
  void Clean();
  void RegisterCleaner(std::function<void()>);
};

#endif//_GLOBAL_H_









