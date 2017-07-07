#include "global.h"

// Global Variables
namespace otv
{
  // objects
  World world; 
  Mesh mesh;
  // flags
  bool NOWIN_FLAG = false;
  // parameters
  int   WINX = 0, WINY = 0;
  vec2i WINSIZE = otv::vec2i(1024, 1024);

};
