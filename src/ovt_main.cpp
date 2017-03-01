#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "otv_common.h"
#include "otv_helper.h"
#include "quad.h"

unsigned int WINX = 0, WINY = 0;
const vec2i imageSize(1024, 1024);
uint32_t* fb;
otv::Quad quad;
otv::Trackball controlball;

/**
* @brief GetMouseButton: Mouse button handling function
* @param button (GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON)
* @param state  (GLUT_UP or GLUT_DOWN)
* @param x
* @param y
*/
void GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
	static cy::Point2f p;
	otv::helper::mouse2screen(x, y, imageSize.x, imageSize.y, p);
	controlball.BeginDrag(p[0], p[1]);
}

/**
* @brief GetMousePosition: Mouse position callbacl handling function
* @param x
* @param y
*/
void GetMousePosition(GLint x, GLint y) {
	static cy::Point2f p;
	otv::helper::mouse2screen(x, y, imageSize.x, imageSize.y, p);
	controlball.Drag(p[0], p[1]);
}

void GetNormalKeys(unsigned char key, GLint x, GLint y) {
	if (key == 27) { glutLeaveMainLoop(); }
}

void render()
{
	quad.Bind(imageSize.x, imageSize.y, fb);
	quad.Draw();
	glutSwapBuffers();
}

int main(int argc, const char **argv)
{
	vec3i volumeDims(256, 256, 256);
	for (int i = 1; i < argc; ++i) {
		if (std::strcmp(argv[i], "-s") == 0) {
			volumeDims.x = std::atoi(argv[++i]);
			volumeDims.y = std::atoi(argv[++i]);
			volumeDims.z = std::atoi(argv[++i]);
			std::cout << "Got volume dims from cmd line: " << volumeDims << "\n";
		}
	}

	const vec3f camPos(-248, -62, 60);
	const vec3f camDir = vec3f(volumeDims) / 2.f - camPos;
	const vec3f camUp(0, 0, 1);

	ospInit(&argc, argv);

	OSPCamera camera = ospNewCamera("perspective");
	ospSetf(camera, "aspect", imageSize.x / static_cast<float>(imageSize.y));
	ospSetVec3f(camera, "pos", (osp::vec3f&)camPos);
	ospSetVec3f(camera, "dir", (osp::vec3f&)camDir);
	ospSetVec3f(camera, "up", (osp::vec3f&)camUp);
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

	OSPRenderer renderer = ospNewRenderer("scivis");
	ospSetObject(renderer, "model", world);
	ospSetObject(renderer, "camera", camera);
	ospCommit(renderer);

	OSPFrameBuffer framebuffer = ospNewFrameBuffer((osp::vec2i&)imageSize, OSP_FB_SRGBA, OSP_FB_COLOR | OSP_FB_ACCUM);
	ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);

	ospRenderFrame(framebuffer, renderer, OSP_FB_COLOR | OSP_FB_ACCUM);

	fb = (uint32_t*)ospMapFrameBuffer(framebuffer, OSP_FB_COLOR);
	otv::helper::writePPM("volume.ppm", imageSize, fb);

	// check argument number
	if (argc < 2) {
		std::cerr << "The program needs at lease one input argument!" << std::endl;
		// return EXIT_FAILURE;
	}

	// initialize everything
	{
		glutInit(&argc, const_cast<char**>(argv));
		glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
		glutInitWindowPosition(WINX, WINY);
		glutInitWindowSize(imageSize.x, imageSize.y);
		glutCreateWindow(argv[0]);
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			std::cerr << "Error: Cannot Initialize GLEW " << glewGetErrorString(err) << std::endl;
			return EXIT_FAILURE;
		}
		quad.Init();
	}

	// execute the program
	{
		glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
		glDisable(GL_DEPTH_TEST);
		glutDisplayFunc(render);
		glutIdleFunc(NULL);
		glutMouseFunc(GetMouseButton);
		glutMotionFunc(GetMousePosition);
		glutKeyboardFunc(GetNormalKeys);
		glutInitContextFlags(GLUT_DEBUG);
		glutMainLoop();
	}
	
	// exit
	ospUnmapFrameBuffer(fb, framebuffer);
	return EXIT_SUCCESS;

}

