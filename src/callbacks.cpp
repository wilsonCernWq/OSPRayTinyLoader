#include "callbacks.h"

using namespace ospcommon;
using namespace otv;

void otv::KeyboardAction(int key, int x, int y)
{
    switch (key) {
    case 27:
	glutLeaveMainLoop();
	break;
    case (int)GLUT_KEY_UP:
	camera.zoom *= .9f;
	camera.Update();
	break;
    case (int)GLUT_KEY_DOWN:
	camera.zoom /= .9f;
	camera.Update();
	break;
    default:
	break;
    }
}

void otv::Camera::Update(bool cleanbuffer)
{
    dir = focus - pos;
    auto currCamUp  = cyPoint3f(Trackball::Matrix() * 
				cyPoint4f((cyPoint3f)up, 0.0f));
    auto currCamDir = cyPoint3f(Trackball::Matrix() * 
				cyPoint4f((cyPoint3f)dir, 0.0f));
    auto currCamPos = (cyPoint3f)focus - currCamDir * zoom;
    ospSetVec3f(ospCamera, "pos", (osp::vec3f&)currCamPos);
    ospSetVec3f(ospCamera, "dir", (osp::vec3f&)currCamDir);
    ospSetVec3f(ospCamera, "up",  (osp::vec3f&)currCamUp);
    ospCommit(ospCamera);
    if (cleanbuffer) {
	ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
    }
}

void otv::GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
    static cy::Point2f p;
    otv::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
    otv::camera.BeginDrag(p[0], p[1]);
}

void otv::GetMousePosition(GLint x, GLint y) {
    static cy::Point2f p;
    otv::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
    otv::camera.Drag(p[0], p[1]);
    otv::camera.Update();
}

void otv::GetNormalKeys(unsigned char key, GLint x, GLint y) {
    KeyboardAction((int) key, x, y);
}

void otv::GetSpecialKeys(int key, GLint x, GLint y) {
    KeyboardAction(key, x, y);
}

void otv::Clean()
{
    gfb.Delete();
    ospUnmapFrameBuffer(ofb, framebuffer);
    ospRelease(world);
    ospRelease(camera.ospCamera);
    ospRelease(renderer);
    ospRelease(framebuffer);
}
