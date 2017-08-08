#include "scenegraph.h"

//________________________________________________________________________________
//
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

//________________________________________________________________________________
//
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

//________________________________________________________________________________
//
otv::SceneGraphAbstract::SgLight::SgLight(otv::Light& l)
{
  SetLight(l);
}

void otv::SceneGraphAbstract::SgLight::SetLight(otv::Light& l)
{
  light = &l;
  // ambient light
  Iamb = l.GetAmbStrength();
  Camb = l.GetAmbColor();
  // directional light
  Idir = l.GetDirStrength();
  Cdir = l.GetDirColor();
  Mdir = l.GetDirLight().Matrix();
  Fdir = l.GetDirLight().GetF();
  Udir = l.GetDirLight().GetU();
  Pdir = l.GetDirLight().GetP();
  Ddir = l.GetDirLight().GetD();
  Zdir = l.GetDirLight().GetZoom();
  // sun light
  Isun = l.GetSunStrength();
  Csun = l.GetSunColor();
  Msun = l.GetSunLight().Matrix();
  Fsun = l.GetSunLight().GetF();
  Usun = l.GetSunLight().GetU();
  Psun = l.GetSunLight().GetP();
  Dsun = l.GetSunLight().GetD();
  Zsun = l.GetSunLight().GetZoom();
}

//________________________________________________________________________________
//
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
}

void
otv::SceneGraphAbstract::PushToWorld()
{
  // camera
  auto& c = world_ptr->GetCamera();
  {
    c.Reset(cameraSg.matrix);
    c.SetZoom(cameraSg.zoom);
    c.SetFocus(cameraSg.focus);
    c.SetPos(cameraSg.pos);
    c.SetUp(cameraSg.up);
    c.Update();
  }
  // light
  auto& l = world_ptr->GetLight();
  {
    // ambient light
    l.SetAmbStrength(lightSg.Iamb);
    l.SetAmbColor(lightSg.Camb);
    // directional light
    l.SetDirStrength(lightSg.Idir);
    l.SetDirColor(lightSg.Cdir);
    l.GetDirLight().Reset(lightSg.Mdir);
    l.GetDirLight().Set(lightSg.Fdir, lightSg.Pdir, lightSg.Udir);
    l.GetDirLight().SetZoom(lightSg.Zdir);
    // sun light
    l.SetSunStrength(lightSg.Isun);
    l.SetSunColor(lightSg.Csun);
    l.GetSunLight().Reset(lightSg.Msun);
    l.GetSunLight().Set(lightSg.Fsun, lightSg.Psun, lightSg.Usun);
    l.GetSunLight().SetZoom(lightSg.Zsun);
    l.Update();
  }
}

void
otv::SceneGraphAbstract::PullFromAll()
{
  // camera
  cameraSg.SetCamera(world_ptr->GetCamera());
  // light
  lightSg.SetLight(world_ptr->GetLight());
  // meshes
  objectsSg.clear();
  for (auto& m : *mesh_ptr) {
    objectsSg.emplace_back(*m);
  }
}
