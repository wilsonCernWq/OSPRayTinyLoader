#include "world.h"

void ::otv::World::Clean()
{
  light.Clean();
  camera.Clean();
  framebuffer.Clean();
  if (this->ospmodel != nullptr) {
    ospRelease(this->ospmodel);
    this->ospmodel = nullptr;
  }
  if (this->osprenderer = nullptr) {
    ospRelease(this->osprenderer);
    this->osprenderer = nullptr;
  }
}
