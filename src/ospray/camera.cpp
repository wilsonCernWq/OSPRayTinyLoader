#include "camera.h"

void otv::Camera::Init(vec2i size) 
{
  ospCam = ospNewCamera("perspective");
  ospSetf(ospCam, "aspect", 
	  static_cast<float>(size.x) / 
	  static_cast<float>(size.y));
  Update();
}

void otv::Camera::Clean() {
  if (ospCam != nullptr) {
    ospRelease(this->ospCam);	 
    ospCam = nullptr;
  }
}

void otv::Camera::Update()
{
  this->dir = this->focus - this->pos;
  auto currCamUp  = vec3f(this->Trackball::Matrix() * vec4f(this->up,  0.0f));
  auto currCamDir = vec3f(this->Trackball::Matrix() * vec4f(this->dir, 0.0f));
  auto currCamPos = this->focus - currCamDir * this->zoom;
  ospSetVec3f(this->ospCam, "pos", (osp::vec3f&)currCamPos);
  ospSetVec3f(this->ospCam, "dir", (osp::vec3f&)currCamDir);
  ospSetVec3f(this->ospCam, "up",  (osp::vec3f&)currCamUp);
  ospCommit(this->ospCam);
}
