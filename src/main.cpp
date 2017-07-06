#include "common.h"
#include "helper.h"
#include "trackball.h"
#include "meshwrapper.h"
#include "callbacks.h"

using namespace otv;

void render()
{
  // render
  world.GetFrameBuffer().RenderOpenGL();
}

void setupospray(const char* meshfile) 
{
  std::cout << "load mesh" << std::endl;
  // geometry/volume
  mesh.LoadFromFileObj(meshfile);

  std::cout << "create ospray world" << std::endl;
  // world
  world.Init(false, ::otv::World::RENDERTYPE::PATHTRACER,
	     mesh, mesh.GetCenter(),
	     otv::mesh.GetDiagonalLength()/10.0f);
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
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(1024, 1024);
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
    //world.Clean();
    return EXIT_SUCCESS;
}
