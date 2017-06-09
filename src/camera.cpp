#include "camera.h"

void otv::Camera::Update(FrameBuffer* framebuffer)
{
    this->dir = this->focus - this->pos;
    auto currCamUp  = cyPoint3f(this->Trackball::Matrix() * 
				cyPoint4f((cyPoint3f)this->up, 0.0f));
    auto currCamDir = cyPoint3f(this->Trackball::Matrix() * 
				cyPoint4f((cyPoint3f)this->dir, 0.0f));
    auto currCamPos = (cyPoint3f)this->focus - currCamDir * this->zoom;
    ospSetVec3f(this->ospCam, "pos", (osp::vec3f&)currCamPos);
    ospSetVec3f(this->ospCam, "dir", (osp::vec3f&)currCamDir);
    ospSetVec3f(this->ospCam, "up",  (osp::vec3f&)currCamUp);
    ospCommit(this->ospCam);
    if (framebuffer != nullptr) {
	framebuffer->ClearFrameBuffer();
    }
}
