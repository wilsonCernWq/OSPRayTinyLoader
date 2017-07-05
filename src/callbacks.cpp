#include "callbacks.h"

using namespace ospcommon;
using namespace otv;

static int mousebutton = -1;

void otv::Light::Update()
{
    // ambient light
    ospSet1f(this->ospAmb, "intensity", Iamb);
    ospCommit(this->ospAmb);
    // directional light
    this->dir = this->focus - this->pos;
    auto currCamUp  = cyPoint3f(this->Trackball::Matrix() * 
				cyPoint4f((cyPoint3f)this->up, 0.0f));
    auto currCamDir = cyPoint3f(this->Trackball::Matrix() * 
				cyPoint4f((cyPoint3f)this->dir, 0.0f));
    auto currCamPos = (cyPoint3f)this->focus - currCamDir * this->zoom;    
    ospSetVec3f(this->ospDir, "direction", (osp::vec3f&)currCamPos);
    ospSet1f(this->ospDir, "intensity", Idir);
    ospSet1f(this->ospDir, "angularDiameter", 53.0f);
    ospSet1i(this->ospDir, "isVisible", 0);
    ospCommit(this->ospDir);
}

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
