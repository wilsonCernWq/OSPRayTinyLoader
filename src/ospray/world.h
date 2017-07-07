#pragma once
#ifndef _WORLD_H_
#define _WORLD_H_

#include "common/helper.h"
#include "loader/meshwrapper.h"

#include "light.h"
#include "camera.h"

namespace otv 
{
  class World {
  public:
    enum RENDERTYPE {SCIVIS, PATHTRACER};
    
  private:
    // parameters
    vec2i size = vec2i(1024,1024);
    uint32_t* buffer = nullptr;
    
    // ospray objects
    OSPModel    ospmodel    = nullptr;    
    OSPRenderer osprenderer = nullptr;
    OSPFrameBuffer ospfb    = nullptr;

    // objects
    Light       light;
    Camera      camera;

  public:
    World() = default;
    ~World() { Clean(); }

    Light& GetLight() { return this->light; }
    Camera& GetCamera() { return this->camera; }

    uint32_t* GetImageData() { return buffer; }

    void CreateFrameBuffer(const vec2i& newsize);
    void CreateModel();
    void CreateRenderer(RENDERTYPE renderType);

    void Init(const vec2i& newsize,
	      RENDERTYPE renderType, otv::Mesh& mesh,
	      vec3f cameraCenter, float cameraZoom);
    
    void Create(int argc, const char **argv) {
      // initialize openGL
      OpenGLCreate(argc, argv);
      // setting up ospray    
      ospInit(&argc, argv);
    }
    void Start() {
      OpenGLStart();      
    }    
    void Clean();
    void ClearFrame() {
      ospFrameBufferClear(ospfb, OSP_FB_COLOR | OSP_FB_ACCUM);
    }
    void Render() {
      ospRenderFrame(ospfb, osprenderer, OSP_FB_COLOR | OSP_FB_ACCUM);
    }

    void (*OpenGLCreate)(int argc, const char **argv);
    void (*OpenGLStart)();

  };
};

#endif//_WORLD_H_
