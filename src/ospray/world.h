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
    vec2i winsize;
    bbox3f bbox;
    uint32_t* fb;
    std::vector<otv::Mesh*> objects;
    // ospray objects
    OSPModel       ospmodel       = nullptr;    
    OSPRenderer    osprenderer    = nullptr;
    OSPFrameBuffer ospframebuffer = nullptr;
    // objects
    Light  light;
    Camera camera;
  private:
    /** \brief Create objects */
    void CreateFrameBuffer(const vec2i& newsize);
    void CreateModel();
    void CreateRenderer(RENDERTYPE renderType);    
  public:
    World();
    ~World() { Clean(); }
    /** \brief Access OSPRay objects */
    Light& GetLight() { return this->light; }
    Camera& GetCamera() { return this->camera; }
    /** \brief Retrieve the rendered framebuffer data*/
    uint32_t* GetImageData() { return fb; }
    /** \brief Initialize ospray */
    void Init(const vec2i& newsize, RENDERTYPE renderType,
	      std::vector<otv::Mesh*>& meshes);
    void Create(int argc, const char **argv);
    void Start();
    void Render();
    void Clean();
    void ClearFrame();
    /** \brief two function pointers to create OpenGL */
    void (*OpenGLCreate)(int argc, const char **argv);
    void (*OpenGLStart)();

  };
};

#endif//_WORLD_H_
