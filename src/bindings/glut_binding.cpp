#include "glut_binding.h"
#include <cyGL.h>

static cyGLRenderBuffer2D gfb;
static int mousebutton = -1; // GLUT_LEFT_BUTTON GLUT_RIGHT_BUTTON GLUT_MIDDLE_BUTTON
static int mousestate  = GLUT_UP; // GLUT_UP GLUT_DOWN

void ::otv::KeyboardAction(int key, int x, int y)
{
  switch (key) {
  case 27:
    if (!NOWIN_FLAG) {     
      gfb.Delete();
    }
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

void ::otv::OpenGLCreateSystem(int argc, const char **argv) {
  if (!NOWIN_FLAG) {
    //! initialize openGL    
    glutInit(&argc, const_cast<char**>(argv));
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow(argv[0]);
    GLenum err = glewInit();
    if (GLEW_OK != err) {
      std::cerr << "Error: Cannot Initialize GLEW " 
		<< glewGetErrorString(err) << std::endl;
      exit(EXIT_FAILURE);
    }
  }
}

void ::otv::OpenGLStartSystem()
{
  if (!NOWIN_FLAG) {
    
    gfb.Initialize(true, 4, world.GetWinSizeX(), world.GetWinSizeY());
    
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

void ::otv::OpenGLRender()
{
  if (!NOWIN_FLAG) {
    world.GetFrameBuffer().RenderOSPRay();
    // put framebuffer to screen
    gfb.BindTexture();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
		    world.GetWinSizeX(), world.GetWinSizeY(), 
		    GL_RGBA, GL_UNSIGNED_BYTE,
		    world.GetFrameBuffer().GetMAPFrameBuffer());
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gfb.GetID());
    glBlitFramebuffer(0, 0, world.GetWinSizeX(), world.GetWinSizeY(), 
		      0, 0, world.GetWinSizeX(), world.GetWinSizeY(), 
		      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glutSwapBuffers();
  }
}
