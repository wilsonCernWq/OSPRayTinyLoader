#pragma once
#ifndef _WORLD_H_
#define _WORLD_H_

#include "common/helper.h"
#include "loader/meshwrapper.h"

#include "light.h"
#include "camera.h"

namespace otv {
  
  class World {
  public:
    enum RENDERTYPE {SCIVIS, PATHTRACER};    
  private:
    // parameters
    bool initialized;
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
    Light&  GetLight() { return this->light; }
    Camera& GetCamera() { return this->camera; }
    /** \brief Retrieve the pointers to mesh */
    std::vector<otv::Mesh*>& GetMeshes() { return this->objects; }
    /** \brief Retrieve the rendered framebuffer data*/
    uint32_t* GetImageData() { return fb; }   
    /** \brief Initialize ospray */
    void Create(int argc, const char **argv);
    void Init(const vec2i& newsize, RENDERTYPE renderType,
	      std::vector<otv::Mesh*>& meshes);
    void Start();
    void Render();
    void Clean();
    void ClearFrame();
    /** \brief Function pointer for creating OpenGL */
    void (*OpenGLStart)();
    /** \brief Helper functions to make the interface easier */
    void ZoomIn() {
      GetCamera().SetZoomIn();
      GetCamera().Update();
      ClearFrame();    
    }
    void ZoomOut() {
      GetCamera().SetZoomOut();
      GetCamera().Update();
      ClearFrame();    
    }
    void BeginDragCamera(float x, float y) {
      GetCamera().BeginDrag(x, y);
    }
    void BeginDragLights(float x, float y) {
      GetLight().GetDirLight().BeginDrag(x, y);	
    }
    void DragCamera(float x, float y) {
      GetCamera().Drag(x, y);
      GetCamera().Update();
      ClearFrame();
    }
    void DragLights(float x, float y) {
      GetLight().GetDirLight().Drag(x, y);
      GetLight().Update();
      GetCamera().Update();
      ClearFrame();
    }

  };
};

#endif//_WORLD_H_
