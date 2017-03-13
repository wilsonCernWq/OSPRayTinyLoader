#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#include "otv_common.h"
#include "otv_helper.h"
#include "otv_trackball.h"
#include "otv_meshwrapper.h"

using namespace ospcommon;

unsigned int WINX = 0, WINY = 0;
const vec2i WINSIZE(1024, 1024);

//! texture maps
uint32_t*          fb_osp;
cyGLRenderBuffer2D fb_gl;

//! OSPRay objects
OSPModel world;
OSPCamera camera;
OSPRenderer renderer;
OSPFrameBuffer framebuffer;

//! camera objects
vec3f camPos(0, -10, 10);
vec3f camDir = vec3f(0,0,0) - camPos;
vec3f camUp(0, 1, 0);
otv::Trackball camRotate(true);

//! mesh
otv::Mesh mesh;

void UpdateCamera(bool cleanbuffer = true) 
{
	auto currCamUp  = cyPoint3f(camRotate.Matrix() * cyPoint4f((cyPoint3f)camUp,  0.0f));
	auto currCamDir = cyPoint3f(camRotate.Matrix() * cyPoint4f((cyPoint3f)camDir, 0.0f));
	auto currCamPos = (cyPoint3f)(vec3f(0, 0, 0)) - currCamDir;
	ospSetVec3f(camera, "pos", (osp::vec3f&)currCamPos);
	ospSetVec3f(camera, "dir", (osp::vec3f&)currCamDir);
	ospSetVec3f(camera, "up",  (osp::vec3f&)currCamUp);
	ospCommit(camera);
	if (cleanbuffer) {
		ospFrameBufferClear(framebuffer, OSP_FB_COLOR | OSP_FB_ACCUM);
	}
}

void GetMouseButton(GLint button, GLint state, GLint x, GLint y) {
	static cy::Point2f p;
	otv::helper::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
	camRotate.BeginDrag(p[0], p[1]);
}

void GetMousePosition(GLint x, GLint y) {
	static cy::Point2f p;
	otv::helper::mouse2screen(x, y, WINSIZE.x, WINSIZE.y, p);
	camRotate.Drag(p[0], p[1]);
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

	mesh.LoadFromFileObj(argv[1]);

	ospInit(&argc, argv);

	camera = ospNewCamera("perspective");
	ospSetf(camera, "aspect", WINSIZE.x / static_cast<float>(WINSIZE.y));
	UpdateCamera(false);

	world = ospNewModel();
	renderer = ospNewRenderer("raytracer");

	int i = 0;
	for (int i = 0; i < mesh.geometries.size(); ++i) {
		if (mesh.geometries[i].num_faces != 0) {
			OSPGeometry geometry_data = ospNewGeometry("triangles");
			//! vertex
			OSPData vertex_data = ospNewData(mesh.geometries[i].vertex.size() / 3, OSP_FLOAT3, mesh.geometries[i].vertex.data(), OSP_DATA_SHARED_BUFFER);
			ospCommit(vertex_data);
			ospSetObject(geometry_data, "vertex", vertex_data);
			//! index
			OSPData index_data = ospNewData(mesh.geometries[i].index.size() / 3, OSP_INT3, mesh.geometries[i].index.data(), OSP_DATA_SHARED_BUFFER);
			ospCommit(index_data);
			ospSetObject(geometry_data, "index", index_data);
			//! normal
			if (mesh.geometries[i].has_normal) {
				OSPData normal_data = ospNewData(mesh.geometries[i].normal.size() / 3, OSP_FLOAT3, mesh.geometries[i].normal.data(), OSP_DATA_SHARED_BUFFER);
				ospCommit(normal_data);
				ospSetObject(geometry_data, "vertex.normal", normal_data);
			}
			//! texture coordinate
			if (mesh.geometries[i].has_texcoord) {
				OSPData texcoord_data = ospNewData(mesh.geometries[i].texcoord.size() / 2, OSP_FLOAT2, mesh.geometries[i].texcoord.data(), OSP_DATA_SHARED_BUFFER);
				ospCommit(texcoord_data);
				ospSetObject(geometry_data, "vertex.texcoord", texcoord_data);
			}
			//! material
			OSPMaterial ospmtl = ospNewMaterial(renderer, "OBJMaterial");


			ospCommit(geometry_data);
			ospAddGeometry(world, geometry_data);
		}
	}
	ospCommit(world);

	//OSPTransferFunction transferFcn = ospNewTransferFunction("piecewise_linear");
	//const std::vector<vec3f> colors = {
	//	vec3f(0, 0, 0.563),vec3f(0, 0, 1),vec3f(0, 1, 1),vec3f(0.5, 1, 0.5),vec3f(1, 1, 0),vec3f(1, 0, 0),vec3f(0.5, 0, 0)
	//};
	//const std::vector<float> opacities = { 0.01f, 0.05f, 0.01f };
	//OSPData colorsData = ospNewData(colors.size(), OSP_FLOAT3, colors.data());
	//ospCommit(colorsData);
	//OSPData opacityData = ospNewData(opacities.size(), OSP_FLOAT, opacities.data());
	//ospCommit(opacityData);
	//const vec2f valueRange(static_cast<float>(0), static_cast<float>(255));
	//ospSetData(transferFcn, "colors", colorsData);
	//ospSetData(transferFcn, "opacities", opacityData);
	//ospSetVec2f(transferFcn, "valueRange", (osp::vec2f&)valueRange);
	//ospCommit(transferFcn);

	//std::vector<unsigned char> volumeData(volumeDims.x * volumeDims.y * volumeDims.z, 0);
	//for (size_t i = 0; i < volumeData.size(); ++i) { volumeData[i] = i % 256; }
	//OSPVolume volume = ospNewVolume("block_bricked_volume");
	//ospSetString(volume, "voxelType", "uchar");
	//ospSetVec3i(volume, "dimensions", (osp::vec3i&)volumeDims);
	//ospSetObject(volume, "transferFunction", transferFcn);
	//ospSetRegion(volume, volumeData.data(), osp::vec3i{ 0, 0, 0 }, (osp::vec3i&)volumeDims);
	//ospSet1i(volume, "singleShade", 0);
	//ospCommit(volume);

	//OSPModel world = ospNewModel();
	//ospAddVolume(world, volume);
	//ospCommit(world);

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

