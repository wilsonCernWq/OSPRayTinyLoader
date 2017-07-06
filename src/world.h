#pragma once
#ifndef _WORLD_H_
#define _WORLD_H_

#include "light.h"
#include "camera.h"
#include "framebuffer.h"
#include "meshwrapper.h"

namespace otv 
{
  class World {
  public:
    enum RENDERTYPE {SCIVIS, PATHTRACER};
    
  private:
    // control flags
    bool NOWIN = false;
    int mousebutton = -1;
    
    // parameters
    unsigned int WINX = 0, WINY = 0;
    vec2i WINSIZE = vec2i(1024, 1024);
      
    // ospray objects
    OSPModel    ospworld    = nullptr;    
    OSPRenderer osprenderer = nullptr;

    // objects
    FrameBuffer framebuffer;
    Light       light;
    Camera      camera;

  public:
    World() = default;
    ~World() { Clean(); }
    
    Camera& GetCamera() { return this->camera; }
    FrameBuffer& GetFrameBuffer() { return this->framebuffer; }
    int GetWindowX() { return WINSIZE.x; }
    int GetWindowY() { return WINSIZE.y; }
    
    void SetRenderer(RENDERTYPE renderType)
    {
      // possible options: "scivis" "pathtracer" "raytracer"
      switch (renderType) {
      case(SCIVIS):
	this->osprenderer = ospNewRenderer("scivis");
	break;
      case(PATHTRACER):
	this->osprenderer = ospNewRenderer("pathtracer");
	break;
      default:
	std::cout << "error renderer" << std::endl;
	break;
      }
      ospSet1i(this->osprenderer, "aoSamples", 16);
      ospSet1i(this->osprenderer, "aoTransparencyEnabled", 1);
      ospSet1i(this->osprenderer, "spp", 1);
      ospSet1i(this->osprenderer, "maxDepth", 10);
      ospSet1i(this->osprenderer, "shadowsEnabled", 1);
      ospSet1f(this->osprenderer, "varianceThreshold", 0.0f);
      //! register lights
      light.Init(this->osprenderer);
      ospCommit(this->osprenderer);
    }
    void Init(bool nowin, RENDERTYPE renderType, otv::Mesh& mesh,
	      vec3f cameraCenter, float cameraZoom)
    {
      // camera
      this->camera.SetFocus(cameraCenter);
      this->camera.SetZoom(cameraZoom);     
      this->camera.Init(this->WINSIZE);

      // create world
      this->ospworld = ospNewModel();
      SetRenderer(renderType);
      mesh.AddToModel(this->ospworld, this->osprenderer);
      ospCommit(this->ospworld);
      ospSetObject(this->osprenderer, "model", this->ospworld);
      ospSetObject(this->osprenderer, "camera", this->camera.GetOSPCamera());
      ospCommit(this->osprenderer);

      // framebuffer
      framebuffer.Init(this->WINSIZE, nowin, this->osprenderer);
      ospCommit(this->osprenderer);
    }
    void KeyboardAction(int key, int x, int y);
    void MouseAction(int button, int state, int x, int y);
    void Clean();
  
  };
};

#endif//_WORLD_H_
