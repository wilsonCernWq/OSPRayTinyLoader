#include "light.h"

void otv::Light::Update()
{
  // ambient light
  {
    ospSet1f(this->ospAmb, "intensity", Iamb);
    ospCommit(this->ospAmb);
  }
  // directional light
  {
    this->dir = this->focus - this->pos;
    auto currCamUp  = vec3f(this->Trackball::Matrix() * vec4f(this->up, 0.0f));
    auto currCamDir = vec3f(this->Trackball::Matrix() * vec4f(this->dir, 0.0f));
    auto currCamPos = this->focus - currCamDir * this->zoom;    
    ospSetVec3f(this->ospDir, "direction", (osp::vec3f&)currCamPos);
    ospSet1f(this->ospDir, "intensity", Idir);
    ospSet1f(this->ospDir, "angularDiameter", 0.0f);
    ospSet1i(this->ospDir, "isVisible", 0);
    ospCommit(this->ospDir);
  }
}
