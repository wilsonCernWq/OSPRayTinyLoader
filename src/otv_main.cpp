#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "otv_common.h"
#include "otv_helper.h"
#include "otv_trackball.h"

unsigned int WINX = 0, WINY = 0;
const vec2i WINSIZE(1024, 1024);

uint32_t*          fb_osp;
cyGLRenderBuffer2D fb_gl;

vec3i volumeDims(256, 256, 256);
vec3f camPos(-248, -62, 60);
vec3f camDir = vec3f(volumeDims) / 2.f - camPos;
vec3f camUp(0, 0, 1);
otv::Trackball controlball;

OSPCamera camera;
OSPRenderer renderer;
OSPFrameBuffer framebuffer;

void UpdateCamera() 
{
	auto currCamDir = cyPoint3f(controlball.Matrix() * cyPoint4f((cyPoint3f)camDir, 0.0f));
	auto currCamPos = (cyPoint3f)(vec3f(volumeDims) / 2.f) - currCamDir;
	ospSetVec3f(camera, "pos", (osp::vec3f&)currCamPos);
	ospSetVec3f(camera, "dir", (osp::vec3f&)currCamDir);
	ospCommit(camera);
	ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
}

/**
* @brief GetMouseButton: Mouse button handling function
* @param button (GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON)
* @param state  (GLUT_UP or GLUT_DOWN)
* @param x
* @param y
*/
void GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
	static cy::Point2f p;
	otv::helper::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
	controlball.BeginDrag(p[0], p[1]);
}

/**
* @brief GetMousePosition: Mouse position callbacl handling function
* @param x
* @param y
*/
void GetMousePosition(GLint x, GLint y) {
	static cy::Point2f p;
	otv::helper::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
	controlball.Drag(p[0], p[1]);
	UpdateCamera();
}

void GetNormalKeys(unsigned char key, GLint x, GLint y) {
	if (key == 27) { glutLeaveMainLoop(); }
}

void Idle() { glutPostRedisplay(); }

void render()
{
	ospRenderFrame(framebuffer, renderer, OSP_FB_COLOR | OSP_FB_ACCUM);
	fb_gl.BindTexture();
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WINSIZE.x, WINSIZE.y, GL_RGBA, GL_UNSIGNED_BYTE, fb_osp);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_gl.GetID());
	glBlitFramebuffer(0, 0, WINSIZE.x, WINSIZE.y, 0, 0, WINSIZE.x, WINSIZE.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glutSwapBuffers();
}

int main(int argc, const char **argv)
{

	ospInit(&argc, argv);

	camera = ospNewCamera("perspective");
	ospSetf(camera, "aspect", WINSIZE.x / static_cast<float>(WINSIZE.y));
	ospSetVec3f(camera, "pos", (osp::vec3f&)camPos);
	ospSetVec3f(camera, "dir", (osp::vec3f&)camDir);
	ospSetVec3f(camera, "up",  (osp::vec3f&)camUp);
	ospCommit(camera);

	OSPTransferFunction transferFcn = ospNewTransferFunction("piecewise_linear");
	const std::vector<vec3f> colors = {
		vec3f(0, 0, 0.563),vec3f(0, 0, 1),vec3f(0, 1, 1),vec3f(0.5, 1, 0.5),vec3f(1, 1, 0),vec3f(1, 0, 0),vec3f(0.5, 0, 0)
	};
	const std::vector<float> opacities = { 0.01f, 0.05f, 0.01f };
	OSPData colorsData = ospNewData(colors.size(), OSP_FLOAT3, colors.data());
	ospCommit(colorsData);
	OSPData opacityData = ospNewData(opacities.size(), OSP_FLOAT, opacities.data());
	ospCommit(opacityData);
	const vec2f valueRange(static_cast<float>(0), static_cast<float>(255));
	ospSetData(transferFcn, "colors", colorsData);
	ospSetData(transferFcn, "opacities", opacityData);
	ospSetVec2f(transferFcn, "valueRange", (osp::vec2f&)valueRange);
	ospCommit(transferFcn);

	std::vector<unsigned char> volumeData(volumeDims.x * volumeDims.y * volumeDims.z, 0);
	for (size_t i = 0; i < volumeData.size(); ++i) { volumeData[i] = i % 255; }
	OSPVolume volume = ospNewVolume("block_bricked_volume");
	ospSetString(volume, "voxelType", "uchar");
	ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
	ospSetObject(volume, "transferFunction", transferFcn);
	ospSetRegion(volume, volumeData.data(), osp::vec3i{ 0, 0, 0 }, (osp::vec3i&)volumeDims);
	ospCommit(volume);

	OSPModel world = ospNewModel();
	ospAddVolume(world, volume);
	ospCommit(world);

	renderer = ospNewRenderer("scivis");
	ospSetObject(renderer, "model", world);
	ospSetObject(renderer, "camera", camera);
	ospCommit(renderer);

	framebuffer = ospNewFrameBuffer((osp::vec2i&)WINSIZE, OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
	ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
	ospRenderFrame(framebuffer, renderer, OSP_FB_COLOR | OSP_FB_ACCUM);
	fb_osp = (uint32_t*)ospMapFrameBuffer(framebuffer, OSP_FB_COLOR);

	// check argument number
	if (argc < 2) {
		std::cerr << "The program needs at lease one input argument!" << std::endl;
	}

	// initialize everything
	{
		glutInit(&argc, const_cast<char**>(argv));
		glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
		glutInitWindowPosition(WINX, WINY);
		glutInitWindowSize(WINSIZE.x, WINSIZE.y);
		glutCreateWindow(argv[0]);
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			std::cerr << "Error: Cannot Initialize GLEW " << glewGetErrorString(err) << std::endl;
			return EXIT_FAILURE;
		}
		fb_gl.Initialize(true, 4, WINSIZE.x, WINSIZE.y);
	}

	// execute the program
	{
		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
		glDisable(GL_DEPTH_TEST);
		glutDisplayFunc(render);
		glutIdleFunc(Idle);
		glutMouseFunc(GetMouseButton);
		glutMotionFunc(GetMousePosition);
		glutKeyboardFunc(GetNormalKeys);
		glutInitContextFlags(GLUT_DEBUG);
		glutMainLoop();
	}
	
	// exit
	ospUnmapFrameBuffer(fb_osp, framebuffer);
	return EXIT_SUCCESS;

}

