#include "global.h"
#include "opengl/glut_binding.h"

bool scripting = false;
std::string scriptfile;
std::vector<std::string> meshfiles;

void help()
{
  std::cout
    << std::endl
    << "USAGE:" << std::endl
    << "  loader <options>" << std::endl 
    << std::endl
    << "BASIC OPTIONS" << std::endl
    << "  -size <x> <y>  Setup window size" << std::endl
    << "  -nw            No window mode" << std::endl
    << "  -s <script>    Load a script file" << std::endl
    << std::endl
    << "GUI OPTIONS" << std::endl
    << "  ESC            Exit window" << std::endl
    << "  Left Click     Rotate mesh object" << std::endl
    << "  Right Click    Rotate light direction" << std::endl
    << std::endl << std::endl;
  exit(EXIT_FAILURE);
}

void arguments(int argc, const char **argv)
{
  std::string arg;
  for (int i = 1; i < argc; ++i) {
    arg = std::string(argv[i]);
    if (arg.compare("-size") == 0)
    {
      if (argc <= (i + 2)) {
	otv::ErrorFatal("[Error] not enough input");
	help();
      }
      else {
	otv::WINSIZE.x = atoi(argv[++i]);
	otv::WINSIZE.y = atoi(argv[++i]);
      }
    }
    else if (arg.compare("-nw") == 0) {
      otv::NOWIN = true;
    }
    else if (arg.compare("-s") == 0) {
      scripting = true;
      if (argc <= (i + 1)) {
	otv::ErrorFatal("Not enough input");
	help();
      }
      scriptfile = std::string(argv[++i]);
    }
    else {
      meshfiles.push_back(arg);
    }     
  }
}

int main(int argc, const char **argv)
{
  //! check argument number
  if (argc < 2) {
    std::cerr << "[Error] The program needs at lease one input argument!"
  	      << std::endl;
    help();
  }
  arguments(argc, argv);

  //____________________________________________________________________
  //
  // call function
  otv::Create(argc, argv);

  // register function handlers
  otv::RegisterCleaner([=]() {
      // debug
      otv::sg.PullFromAll();
      otv::sg.Dump("compare");
      // cleaning
      std::cout << "[ospray] cleaning" << std::endl;      
      otv::world.Clean();
      for (auto& m : otv::meshes) { delete m; m = nullptr; }      
    });
  // SG
  otv::sg.SetMeshes(otv::meshes);
  otv::sg.SetWorld(otv::world);
  if (scripting) {
    otv::sg.Load(scriptfile);
  }
  // geometry/volume
  otv::meshes.resize(meshfiles.size());
  for (size_t i = 0; i < meshfiles.size(); ++i)
  {
    otv::meshes[i] = new otv::Mesh();
    if (!otv::meshes[i]->LoadFromFileObj(meshfiles[i].c_str())) {
      otv::ErrorFatal("Fatal error, terminating the program ...");
    }
    // this should be called after loading
    otv::meshes[i]->SetTransform(); 
  }
  if (scripting) {
    otv::sg.PushToMeshes();
  }
  // world
  otv::world.Init(otv::WINSIZE, otv::NOWIN, otv::World::RENDERTYPE::PATHTRACER, otv::meshes);
  if (scripting) {
    otv::sg.PushToWorld();
  }
  otv::world.Start();
  //____________________________________________________________________
  //

  // exit
  return EXIT_SUCCESS;
}
