#include "light.h"

::otv::Light::Light () {
  Iamb = 0.50f;
  Idir = 1.50f;
  Isun = 0.90f;
  Camb = vec3f(174.f/255.f,218.f/255.f,255.f/255.f);
  Cdir = vec3f(127.f/255.f,178.f/255.f,255.f/255.f);
  Csun = vec3f(1.f,232.f/255.f,166.f/255.f);      
  ptDir.focus = vec3f(0.0f);
  ptDir.p     = vec3f(-.93,-.54f,-.605f);
  ptDir.u     = vec3f(0.0f, 1.0f, 0.0f);
  ptDir.d = ptDir.focus - ptDir.p;
  ptSun.focus = vec3f(0.0f);
  ptSun.p     = vec3f(0.462f,-1.f,-.1f);
  ptSun.u     = vec3f(0.0f, 1.0f, 0.0f);
  ptSun.d = ptSun.focus - ptSun.p;
  }

void ::otv::Light::Init(OSPRenderer renderer) {
  if (ospAmb != nullptr) { ospRelease(ospAmb); }
  ospAmb = ospNewLight(renderer, "AmbientLight");
  if (ospDir != nullptr) { ospRelease(ospDir); }
  ospDir = ospNewLight(renderer, "DirectionalLight");
  if (ospSun != nullptr) { ospRelease(ospSun); }
  ospSun = ospNewLight(renderer, "DirectionalLight");
  Update();
  // setup light data
  std::vector<OSPLight> veclights = {ospAmb, ospDir, ospSun};
  osplights = ospNewData(veclights.size(), OSP_OBJECT, veclights.data());
  ospCommit(osplights);
}

void ::otv::Light::Update()
{
  // ambient light
  {
    ospSet1f(this->ospAmb, "intensity", Iamb);
    ospSet1i(this->ospAmb, "isVisible", 0);
    ospSetVec3f(this->ospAmb, "color", (osp::vec3f&)Camb);
    ospCommit(this->ospAmb);
  }
  // directional light
  {
    ptDir.Update();
    ospSetVec3f(this->ospDir, "direction", (osp::vec3f&)ptDir.cp);
    ospSet1f(this->ospDir, "angularDiameter", 0.0f);
    ospSet1f(this->ospDir, "intensity", Idir);
    ospSet1i(this->ospDir, "isVisible", 0);
    ospSetVec3f(this->ospDir, "color", (osp::vec3f&)Cdir);
    ospCommit(this->ospDir);
  }
  // sun light
  {
    ptSun.Update();
    ospSetVec3f(this->ospSun, "direction", (osp::vec3f&)ptSun.cp);
    ospSet1f(this->ospSun, "angularDiameter", 53.0f);
    ospSet1f(this->ospSun, "intensity", Isun);
    ospSet1i(this->ospSun, "isVisible", 0);
    ospSetVec3f(this->ospSun, "color", (osp::vec3f&)Csun);
    ospCommit(this->ospSun);
  }

}
