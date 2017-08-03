#include "glut_binding.h"
#include <cyGL.h>

// parameters
static cyGLRenderBuffer2D gfb;
static int mousebutton = -1; // GLUT_LEFT_BUTTON GLUT_RIGHT_BUTTON GLUT_MIDDLE_BUTTON
static int mousestate  = GLUT_UP; // GLUT_UP GLUT_DOWN

void KeyboardAction(int key, int x, int y);
void MouseAction(int button, int state, int x, int y);
void GetNormalKeys(unsigned char key, GLint x, GLint y);
void GetSpecialKeys(int key, GLint x, GLint y);
void GetMouseButton(GLint button, GLint state, GLint x, GLint y);
void GetMousePosition(GLint x, GLint y);
void OpenGLCreateSystem(int argc, const char **argv);
void OpenGLStartSystem();
void OpenGLRender();
void Idle();

void KeyboardAction(int key, int x, int y)
{
  switch (key) {
  case 27:
    if (!otv::NOWIN) {
      // this is just a hack to stop the rendering loop
      // a better way is to setup a new flag to control it
      // but this way should work currently
      otv::NOWIN = true;
      otv::Clean();
      gfb.Delete();
    }
    glutLeaveMainLoop();
    break;
  case (int)GLUT_KEY_UP:
    otv::world.ZoomIn();
    break;
  case (int)GLUT_KEY_DOWN:
    otv::world.ZoomOut();
    break;
  default:
    break;
  }
}

void MouseAction(int button, int state, int x, int y) {
  static otv::vec2f p;
  otv::mouse2screen(x, y, otv::WINSIZE.x, otv::WINSIZE.y, p);
  if (state == GLUT_UP) {
    if (button == GLUT_LEFT_BUTTON) {
      otv::world.BeginDragCamera(p[0], p[1]);	
    } 
    else if (button == GLUT_RIGHT_BUTTON) {
      otv::world.BeginDragLights(p[0], p[1]);	
    }
  }
  else {
    if (button == GLUT_LEFT_BUTTON) {
      otv::world.DragCamera(p[0], p[1]);
    } 
    else if (button == GLUT_RIGHT_BUTTON) {
      otv::world.DragLights(p[0], p[1]);
    }
  }
}

void GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
  MouseAction(button, mousestate, x, y);
  mousebutton = button;
  mousestate = state;
}

void GetMousePosition(GLint x, GLint y) {
  MouseAction(mousebutton, mousestate, x, y);
}

void GetNormalKeys(unsigned char key, GLint x, GLint y) {
  KeyboardAction((int) key, x, y);
}

void GetSpecialKeys(int key, GLint x, GLint y) {
  KeyboardAction(key, x, y);
}

void Idle() { glutPostRedisplay(); }

void OpenGLCreateSystem(int argc, const char **argv) {
  if (!otv::NOWIN) {
    glutInit(&argc, const_cast<char**>(argv));
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(otv::WINX, otv::WINY);
    glutInitWindowSize(otv::WINSIZE.x, otv::WINSIZE.y);
    glutCreateWindow(argv[0]);
    GLenum err = glewInit();
    if (GLEW_OK != err) {
      std::cerr << "Error: Cannot Initialize GLEW " 
		<< glewGetErrorString(err) << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

void OpenGLStartSystem()
{
  if (!otv::NOWIN) {    
    gfb.Initialize(true, 4, otv::WINSIZE.x, otv::WINSIZE.y);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glDisable(GL_DEPTH_TEST);
    glutDisplayFunc(OpenGLRender);
    glutIdleFunc(Idle);
    glutMouseFunc(GetMouseButton);
    glutMotionFunc(GetMousePosition);
    glutKeyboardFunc(GetNormalKeys);
    glutSpecialFunc(GetSpecialKeys);
    glutInitContextFlags(GLUT_DEBUG);
    glutMainLoop();
  }
}

void OpenGLRender()
{
    if (!otv::NOWIN) {
    otv::world.Render();
    // put framebuffer to screen
    gfb.BindTexture();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, otv::WINSIZE.x, otv::WINSIZE.y, 
		    GL_RGBA, GL_UNSIGNED_BYTE,
		    otv::world.GetImageData());
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gfb.GetID());
    glBlitFramebuffer(0, 0, otv::WINSIZE.x, otv::WINSIZE.y, 
		      0, 0, otv::WINSIZE.x, otv::WINSIZE.y, 
		      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glutSwapBuffers();
  }
}

// implementation of global function
void GLUT_Init(int argc, const char **argv) {
  otv::world.OpenGLStart = OpenGLStartSystem;
  OpenGLCreateSystem(argc, argv);
}
bool GLUT_Setup() {
    otv::Init = GLUT_Init;
    return true;
}

namespace otv {
    namespace GLUT {
	bool initialized = GLUT_Setup();
    };
};

