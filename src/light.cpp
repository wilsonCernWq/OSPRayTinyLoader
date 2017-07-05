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
    auto currCamUp  = cyPoint3f(this->Trackball::Matrix() * 
				cyPoint4f((cyPoint3f)this->up, 0.0f));
    auto currCamDir = cyPoint3f(this->Trackball::Matrix() * 
				cyPoint4f((cyPoint3f)this->dir, 0.0f));
    auto currCamPos = (cyPoint3f)this->focus - currCamDir * this->zoom;    
    ospSetVec3f(this->ospDir, "direction", (osp::vec3f&)currCamPos);
    ospSet1f(this->ospDir, "intensity", Idir);
    ospSet1f(this->ospDir, "angularDiameter", 0.0f);
    ospSet1i(this->ospDir, "isVisible", 0);
    ospCommit(this->ospDir);
  }
}
