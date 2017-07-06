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

void otv::GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
  world.MouseAction(button, mousestate, x, y);
  mousebutton = button;
  mousestate = state;
}

void otv::GetMousePosition(GLint x, GLint y) {
  world.MouseAction(mousebutton, mousestate, x, y);
}

void otv::GetNormalKeys(unsigned char key, GLint x, GLint y) {
  world.KeyboardAction((int) key, x, y);
}

void otv::GetSpecialKeys(int key, GLint x, GLint y) {
  world.KeyboardAction(key, x, y);
}
