#pragma once
#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

#include "common/helper.h"
#include "loader/meshwrapper.h"
#include "ospray/world.h"

namespace otv {
  /** \brief structure to store object location in world coordinates */
  class Mesh;
  struct SceneGraph {
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
      };
      bbox3f bbox; // manually define bounding box
      SgObject(otv::Mesh& m) {
	mesh       = &m;	
	filename   = m.GetFullPath();
	bbox.upper = m.GetBBoxMax();
	bbox.lower = m.GetBBoxMin();
	T = m.GetTransform().p;
	R = m.GetTransform().l;
      }
    };
    struct SgCamera {
      Camera* camera;
      vec3f focus;
      vec3f dir;
      vec3f pos;
      vec3f up;
      float zoom;
      enum {PERSPECTIVE, ORTHOGRAPHIC} type; // not supported yet
      void SetCamera(otv::Camera& c) {
	camera = &c;
	focus  = c.GetFocus();
	pos    = c.GetPos();
	dir    = c.GetDir();
	up     = c.GetUp();
	zoom   = c.GetZoom();
      }
    };
    std::vector<SgObject> objectsSg;
    SgCamera cameraSg;

    // associated pointer to the cpp class
    otv::World* world_ptr = nullptr;

    void SetWorld(otv::World& world) { world_ptr = &world; }
    void PushToWorld() {}
    void PullFromWorld() {
      // camera
      cameraSg.SetCamera(world_ptr->GetCamera());
      // meshes
      objectsSg.clear();
      for (auto& m : world_ptr->GetMeshes()) {
	objectsSg.emplace_back(*m);	
      }
    }
    void Dump();
  };
};

#endif//_SCENEGRAPH_H_
