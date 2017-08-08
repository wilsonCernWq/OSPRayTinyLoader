#include "scenegraph.h"

otv::SceneGraphAbstract::SgObject::SgObject(otv::Mesh& m)
{
  SetObject(m);
}

void
otv::SceneGraphAbstract::SgObject::SetObject(otv::Mesh& m)
{
  mesh       = &m;
  filename   = m.GetFullPath();
  bbox.upper = m.GetBBoxMax();
  bbox.lower = m.GetBBoxMin();
  center     = m.GetCenter();
  T = m.GetTransform().p;
  R = m.GetTransform().l;
}

otv::SceneGraphAbstract::SgCamera::SgCamera(otv::Camera& c)
{
  SetCamera(c);
}

void
otv::SceneGraphAbstract::SgCamera::SetCamera(otv::Camera& c)
{
  camera = &c;
  matrix = c.Matrix();
  focus  = c.GetFocus();
  pos    = c.GetPos();
  dir    = c.GetDir();
  up     = c.GetUp();
  zoom   = c.GetZoom();
}

void 
otv::SceneGraphAbstract::SetMeshes(std::vector<Mesh*>& mesh)
{ mesh_ptr = &mesh; }

void
otv::SceneGraphAbstract::SetWorld(otv::World& world)
{ world_ptr = &world; }

void
otv::SceneGraphAbstract::PushToMeshes()
{
  mesh_ptr->resize(objectsSg.size());
  for (size_t i = 0; i < mesh_ptr->size(); ++i)
  {
    (*mesh_ptr)[i] = new otv::Mesh();
    if (!(*mesh_ptr)[i]->LoadFromFileObj(objectsSg[i].filename.c_str())) {
      otv::ErrorFatal("Fatal error, terminating the program ...");
    }
    // this should be called after loading
    otv::mat4f matrix = otv::mat4f(vec4f(objectsSg[i].Rv[0], 0.0f), 
				   vec4f(objectsSg[i].Rv[1], 0.0f), 
				   vec4f(objectsSg[i].Rv[2], 0.0f), 
				   vec4f(objectsSg[i].T,     1.0f));
    // std::cout << glm::to_string(matrix) << std::endl;
    (*mesh_ptr)[i]->SetTransform(matrix); 
  }
  // HOLD;
}

void
otv::SceneGraphAbstract::PushToWorld()
{
  world_ptr->GetCamera().Reset(cameraSg.matrix);
  world_ptr->GetCamera().SetZoom(cameraSg.zoom);
  world_ptr->GetCamera().SetFocus(cameraSg.focus);
  world_ptr->GetCamera().SetPos(cameraSg.pos);
  world_ptr->GetCamera().SetUp(cameraSg.up);
  world_ptr->GetCamera().Update();
  // std::cout << "[loading sg] mat  : " << glm::to_string(cameraSg.matrix) << std::endl
  // 	    << "                    " << glm::to_string(world_ptr->GetCamera().Matrix()) << std::endl
  // 	    << "[loading sg] zoom : " << cameraSg.zoom << std::endl
  // 	    << "                    " << world_ptr->GetCamera().GetZoom() << std::endl
  // 	    << "[loading sg] focus: " << glm::to_string(cameraSg.focus) << std::endl
  // 	    << "                    " << glm::to_string(world_ptr->GetCamera().GetFocus()) << std::endl
  // 	    << "[loading sg] pos  : " << glm::to_string(cameraSg.pos)   << std::endl
  // 	    << "                    " << glm::to_string(world_ptr->GetCamera().GetPos()) << std::endl
  // 	    << "[loading sg] dir  : " << glm::to_string(cameraSg.dir)   << std::endl
  // 	    << "                    " << glm::to_string(world_ptr->GetCamera().GetDir()) << std::endl
  // 	    << "[loading sg] up   : " << glm::to_string(cameraSg.up)    << std::endl
  // 	    << "                    " << glm::to_string(world_ptr->GetCamera().GetUp()) << std::endl;
}

void
otv::SceneGraphAbstract::PullFromAll()
{
  // camera
  cameraSg.SetCamera(world_ptr->GetCamera());
  // meshes
  objectsSg.clear();
  for (auto& m : *mesh_ptr) {
    objectsSg.emplace_back(*m);
  }
}
