#include "callbacks.h"

using namespace ospcommon;
using namespace otv;

// Global Variables
namespace otv 
{
    // window size
    unsigned int WINX = 0, WINY = 0;
    ospcommon::vec2i WINSIZE(1024, 1024);

    // OSPRay objects
    OSPModel       world;
    OSPRenderer    renderer;

    // framebuffer object
    FrameBuffer framebuffer;
    
    // light object
    Light light;

    // camera object
    Camera camera;

    // mesh
    Mesh mesh;
};

static int mousebutton = -1;

void otv::KeyboardAction(int key, int x, int y)
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

void otv::GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
    static cy::Point2f p;    
    otv::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
    if (button == GLUT_LEFT_BUTTON) {
	otv::camera.BeginDrag(p[0], p[1]);	
    } 
    else if (button == GLUT_RIGHT_BUTTON) {
	otv::light.BeginDrag(p[0], p[1]);	
    }
    mousebutton = button;
}

void otv::GetMousePosition(GLint x, GLint y) {
    static cy::Point2f p;
    otv::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
    if (mousebutton == GLUT_LEFT_BUTTON) {
	otv::camera.Drag(p[0], p[1]);
	otv::camera.Update(&framebuffer);	
    } 
    else if (mousebutton == GLUT_RIGHT_BUTTON) {
	otv::light.Drag(p[0], p[1]);
	otv::light.Update();
	otv::camera.Update(&framebuffer);	
    }
}

void otv::GetNormalKeys(unsigned char key, GLint x, GLint y) {
    KeyboardAction((int) key, x, y);
}

void otv::GetSpecialKeys(int key, GLint x, GLint y) {
    KeyboardAction(key, x, y);
}

void otv::Clean()
{
    framebuffer.Clean();
    camera.Clean();
    ospRelease(world);
    ospRelease(renderer);
}
