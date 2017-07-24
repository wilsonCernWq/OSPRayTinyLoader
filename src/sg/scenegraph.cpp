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
  focus  = c.GetFocus();
  pos    = c.GetPos();
  dir    = c.GetDir();
  up     = c.GetUp();
  zoom   = c.GetZoom();
}

void
otv::SceneGraphAbstract::SetWorld(otv::World& world)
{ world_ptr = &world; }

void
otv::SceneGraphAbstract::PushToWorld()
{
  HOLD;
}

void
otv::SceneGraphAbstract::PullFromWorld()
{
  // camera
  cameraSg.SetCamera(world_ptr->GetCamera());
  // meshes
  objectsSg.clear();
  for (auto& m : world_ptr->GetMeshes()) {
    objectsSg.emplace_back(*m);	
  }
}
