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
	camZoom *= .9f;
	UpdateCamera();
	break;
    case (int)GLUT_KEY_DOWN:
	camZoom /= .9f;
	UpdateCamera();
	break;
    default:
	break;
    }
}

void otv::UpdateCamera(bool cleanbuffer)
{
    camDir = camFocus - camPos;
    auto currCamUp = cyPoint3f(camRotate.Matrix() * cyPoint4f((cyPoint3f)camUp, 0.0f));
    auto currCamDir = cyPoint3f(camRotate.Matrix() * cyPoint4f((cyPoint3f)camDir, 0.0f));
    auto currCamPos = (cyPoint3f)camFocus - currCamDir * camZoom;
    ospSetVec3f(camera, "pos", (osp::vec3f&)currCamPos);
    ospSetVec3f(camera, "dir", (osp::vec3f&)currCamDir);
    ospSetVec3f(camera, "up",  (osp::vec3f&)currCamUp);
    ospCommit(camera);
    if (cleanbuffer) {
	ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
    }
}

void otv::GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
    static cy::Point2f p;
    otv::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
    camRotate.BeginDrag(p[0], p[1]);
}

void otv::GetMousePosition(GLint x, GLint y) {
    static cy::Point2f p;
    otv::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
    camRotate.Drag(p[0], p[1]);
    UpdateCamera();
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
    ospRelease(camera);
    ospRelease(renderer);
    ospRelease(framebuffer);
}
