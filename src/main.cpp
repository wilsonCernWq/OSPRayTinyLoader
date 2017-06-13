#include "common.h"
#include "helper.h"
#include "trackball.h"
#include "meshwrapper.h"
#include "callbacks.h"

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

using namespace otv;

void render()
{
    // render
    framebuffer.RenderOpenGL();
}

void setupospray(const char* meshfile) 
{
    //! create world and renderer
    world = ospNewModel();

    // possible options: "scivis" "pathtracer" "raytracer"
    renderer = ospNewRenderer("pathtracer");
    //renderer = ospNewRenderer("scivis");

    //! geometry/volume
    mesh.LoadFromFileObj(meshfile);
    mesh.AddToModel(world, renderer);
    ospCommit(world);
    otv::camera.SetFocus(otv::make_vec(mesh.GetCenter()));
    otv::camera.SetZoom(otv::mesh.GetDiagonalLength()/10.0f);

    //! camera
    otv::camera.Init(WINSIZE);

    //! lighting
    light.Init(renderer);

    //! renderer
    ospSetObject(renderer, "model", world);
    ospSetObject(renderer, "camera", camera.GetOSPCamera());

    //! scivis
    ospSet1i(renderer, "shadowsEnabled", 1);
    ospSet1i(renderer, "aoSamples", 2);
    ospSet1i(renderer, "aoTransparencyEnabled", 1);
    //! pathtracer
    ospSet1i(renderer, "spp", 1);
    ospSet1i(renderer, "maxDepth", 10);
    ospSet1f(renderer, "varianceThreshold", 0.0f);

    ospCommit(renderer);

    // framebuffer
    framebuffer.InitRenderer(renderer, WINSIZE);
    
}

int main(int argc, const char **argv)
{
    //! check argument number
    if (argc < 2) {
	std::cerr << "The program needs at lease one input argument!"
		  << std::endl;
	exit(EXIT_FAILURE);
    }

    //! initialize openGL
    {
	glutInit(&argc, const_cast<char**>(argv));
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(WINX, WINY);
	glutInitWindowSize(WINSIZE.x, WINSIZE.y);
	glutCreateWindow(argv[0]);
	GLenum err = glewInit();
	if (GLEW_OK != err) {
	    std::cerr << "Error: Cannot Initialize GLEW " 
		      << glewGetErrorString(err) << std::endl;
	    return EXIT_FAILURE;
	}
    }

    //! setting up ospray
    {
	ospInit(&argc, argv);
	setupospray(argv[1]);
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
	glutSpecialFunc(GetSpecialKeys);
	glutInitContextFlags(GLUT_DEBUG);
	glutMainLoop();
    }

    // exit
    Clean();
    return EXIT_SUCCESS;
}
