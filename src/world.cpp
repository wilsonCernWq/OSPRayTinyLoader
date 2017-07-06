#include "world.h"

void ::otv::World::KeyboardAction(int key, int x, int y)
{
  switch (key) {
  case 27:
    glutLeaveMainLoop();
    break;
  case (int)GLUT_KEY_UP:
    camera.SetZoomIn();
    camera.Update(&framebuffer);
    break;
  case (int)GLUT_KEY_DOWN:
    camera.SetZoomOut();
    camera.Update(&framebuffer);
    break;
  default:
    break;
  }
}

void ::otv::World::MouseAction(int button, int state, int x, int y) {
  static cy::Point2f p;
  ::otv::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
  if (state == GLUT_UP) {
    if (button == GLUT_LEFT_BUTTON) {
      this->camera.BeginDrag(p[0], p[1]);	
    } 
    else if (button == GLUT_RIGHT_BUTTON) {
      this->light.BeginDrag(p[0], p[1]);	
    }
  }
  else {
    if (button == GLUT_LEFT_BUTTON) {
      this->camera.Drag(p[0], p[1]);
      this->camera.Update(&framebuffer);	
    } 
    else if (button == GLUT_RIGHT_BUTTON) {
      this->light.Drag(p[0], p[1]);
      this->light.Update();
      this->camera.Update(&framebuffer);	
    }
  }
}

void ::otv::World::Clean()
{
  framebuffer.Clean();
  camera.Clean();
  if (this->ospworld != nullptr) {
    ospRelease(this->ospworld);
  }
  if (this->osprenderer = nullptr) {
    ospRelease(this->osprenderer);
  }
  this->ospworld = nullptr;
  this->osprenderer = nullptr;
}
