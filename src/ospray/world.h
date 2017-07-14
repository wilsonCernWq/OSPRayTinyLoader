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
    Light  light;
    Camera camera;
  public:
    World() = default;
    ~World() { Clean(); }
    /** \brief Access OSPRay objects */
    Light& GetLight() { return this->light; }
    Camera& GetCamera() { return this->camera; }
    /** \brief Retrieve the rendered framebuffer data*/
    uint32_t* GetImageData() { return buffer; }
    /** \brief Create objects */
    void CreateFrameBuffer(const vec2i& newsize);
    void CreateModel();
    void CreateRenderer(RENDERTYPE renderType);
    /** \brief Initialize ospray */
    void Init(const vec2i& newsize, RENDERTYPE renderType, otv::Mesh& mesh,
	      vec3f cameraCenter, float cameraZoom);    
    void Create(int argc, const char **argv);
    void Start();
    void Clean();
    void ClearFrame();
    void Render();
    /** \brief two function pointers to create OpenGL */
    void (*OpenGLCreate)(int argc, const char **argv);
    void (*OpenGLStart)();

  };
};

#endif//_WORLD_H_
