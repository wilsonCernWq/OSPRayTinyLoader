#pragma once
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "common/helper.h"
#include "loader/meshwrapper.h"
#include "ospray/world.h"

/* choose a JSON implementation */
#include "sg/sgRapidJSON.h"

#include <functional>

namespace otv 
{
  /* global objects */
  extern World world;
  extern std::vector<Mesh*> meshes;
  extern SceneGraph sg;
  
  /* global parameters */
  extern bool  NOWIN;
  extern int   WINX, WINY;
  extern vec2i WINSIZE;
  
  /* global functions */
  //! @name Create ospray and openGL contexts
  //! defined in openGL implementation
  // void Init(int argc, const char **argv);

  //! @name Cleanup everything 
  //! defined in global.cpp  
  void Clean();
  void RegisterCleaner(std::function<void()>);
};

#endif//_GLOBAL_H_









