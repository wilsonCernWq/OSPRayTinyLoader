#include "callbacks.h"

// Global Variables
namespace otv
{
  // control flags
  World world;
  
  // mesh
  Mesh mesh;
};

static int mousebutton = -1; // GLUT_LEFT_BUTTON GLUT_RIGHT_BUTTON GLUT_MIDDLE_BUTTON
static int mousestate  = GLUT_UP; // GLUT_UP GLUT_DOWN

void ::otv::KeyboardAction(int key, int x, int y)
{
  switch (key) {
  case 27:
    glutLeaveMainLoop();
    break;
  case (int)GLUT_KEY_UP:
    world.GetCamera().SetZoomIn();
    world.GetCamera().Update(&world.GetFrameBuffer());
    break;
  case (int)GLUT_KEY_DOWN:
    world.GetCamera().SetZoomOut();
    world.GetCamera().Update(&world.GetFrameBuffer());
    break;
  default:
    break;
  }
}

void ::otv::MouseAction(int button, int state, int x, int y) {
  static cy::Point2f p;
  ::otv::mouse2screen(x, y, world.GetWinSizeX(), world.GetWinSizeY(), p);
  if (state == GLUT_UP) {
    if (button == GLUT_LEFT_BUTTON) {
      world.GetCamera().BeginDrag(p[0], p[1]);	
    } 
    else if (button == GLUT_RIGHT_BUTTON) {
      world.GetLight().GetDirLight().BeginDrag(p[0], p[1]);	
    }
  }
  else {
    if (button == GLUT_LEFT_BUTTON) {
      world.GetCamera().Drag(p[0], p[1]);
      world.GetCamera().Update(&world.GetFrameBuffer());	
    } 
    else if (button == GLUT_RIGHT_BUTTON) {
      world.GetLight().GetDirLight().Drag(p[0], p[1]);
      world.GetLight().Update();
      world.GetCamera().Update(&world.GetFrameBuffer());	
    }
  }
}

void ::otv::GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
  world.MouseAction(button, mousestate, x, y);
  mousebutton = button;
  mousestate = state;
}

void ::otv::GetMousePosition(GLint x, GLint y) {
  world.MouseAction(mousebutton, mousestate, x, y);
}

void ::otv::GetNormalKeys(unsigned char key, GLint x, GLint y) {
  world.KeyboardAction((int) key, x, y);
}

void ::otv::GetSpecialKeys(int key, GLint x, GLint y) {
  world.KeyboardAction(key, x, y);
}
