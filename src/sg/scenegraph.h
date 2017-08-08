#pragma once
#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "../common/helper.h"
#include "../loader/meshwrapper.h"
#include "../ospray/world.h"

namespace otv {
  /** \brief structure to store object location in world coordinates */
  class SceneGraphAbstract {
  public:
    struct SgObject {
      Mesh* mesh = nullptr;
      std::string filename;
      union { // not supported yet
	vec3f translation;
	vec3f T;
      };
      union { // not supported yet
	mat3f rotation;
	mat3f R;
	vec3f Rv[3];
      };
      bbox3f bbox; // manually define bounding box
      vec3f center; 
      SgObject() {};
      SgObject(otv::Mesh& m);
      void SetObject(otv::Mesh& m);
    };
    struct SgCamera {
      Camera* camera;
      mat4f matrix;
      vec3f focus;
      vec3f dir;
      vec3f pos;
      vec3f up;
      float zoom;
      enum {PERSPECTIVE, ORTHOGRAPHIC} type; // not supported yet
      SgCamera() = default;
      SgCamera(otv::Camera& c);
      void SetCamera(otv::Camera& c);
    };    
  public: // this should be private, well ...
    std::vector<SgObject> objectsSg;
    SgCamera cameraSg;
  protected:
    // associated pointer to the cpp class
    std::vector<Mesh*>* mesh_ptr = nullptr; 
    otv::World* world_ptr = nullptr; 
  public:    
    void SetMeshes(std::vector<Mesh*>& mesh);
    void SetWorld(otv::World& world);
    void PushToMeshes();
    void PushToWorld();
    void PullFromAll();
    /* Implemented by different JSON implementations  */
    virtual void Dump(const std::string& fname = "") = 0; 
    virtual void Load(const std::string& fname = "") = 0;
  };
};

#endif//_SCENEGRAPH_H_
