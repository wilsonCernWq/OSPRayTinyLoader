#include "global.h"

// Global Variables
namespace otv
{
  // objects
  World world; 
  std::vector<Mesh*> meshes;
  // flags
  bool NOWIN_FLAG = false;
  // parameters
  int   WINX = 0, WINY = 0;
  vec2i WINSIZE = otv::vec2i(1024, 1024);
  // function handlers
  std::vector<std::function<void()>> cleanlist;
};
